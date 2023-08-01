/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_sleep.h"

#include <esp_http_server.h>
#include "esp_system.h"
 
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
#include "esp_vfs.h"
#include "cJSON.h"

#include "wifi_connect.h"
#include "simple_server.h"
#include "uart_sender.h"
#include "pcnt_controller.h"
#include "pwm_controller.h"
#include "timer_controller.h"
#include "PID_controller.h"

UARTCONF uart_conf = {1, 115200};
PCNTCONF pcnt_conf;
timerConf timer_conf;
PWMGROUP pwm_group;

pidInfo pid_conf = {
    1.2, 0, 0.8, 0.015, 0.002, 0, 0, 0.1, 0.0
};

typedef struct {
    int count;
} element_t;

SERVERCONF server_config = {
    .port = 8278,
    .uri_num = 2,
    .root_path = "/www",
    .index_file = "/index.html",
};

static bool IRAM_ATTR timer_work(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    int pulse_count = 0;
    pcnt_get_num(&pcnt_conf, 0, &pulse_count);
    element_t ele = {
        .count = pulse_count
    };
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
}

static void pwm_task(void* arg)
{
    element_t ele;
    char msg[64];
    for(;;) {
        if(xQueueReceive(timer_conf.queue, &ele, pdMS_TO_TICKS(2000))) {
            // ESP_LOGI(TAG, "%d", ele.count);
            pid_conf.current = ((float)ele.count)/450.0;
            // uart_send(&uart_conf, msg);
            float value = pid_update(&pid_conf);
            sprintf(msg, "%.2f,%.2f,%.2f,%.2f\n", pid_conf.aim-0.2, pid_conf.current, value, pid_conf.previous_error);
            // ESP_LOGI("REST", "aim: %f", pid_conf.aim);
            pwm_update(&pwm_group, 0, value);
        }
    }
}


static esp_err_t post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char buf[1024];
    int received = 0;
    if (total_len >= 10000) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    float value = (float)cJSON_GetObjectItem(root, "v")->valuedouble;
    pid_conf.aim = value;
    ESP_LOGI("REST", "value: v = %f", value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

httpd_uri_t post_uri = {
    .uri = "/api/v1/pid/change_aim",
    .method = HTTP_POST,
    .handler = post_handler,
};

static esp_err_t pid_info_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "error", pid_conf.previous_error);
    cJSON_AddNumberToObject(root, "pwm", pid_conf.pwm);
    cJSON_AddNumberToObject(root, "aim", pid_conf.aim);
    cJSON_AddNumberToObject(root, "speed", pid_conf.current);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

httpd_uri_t pid_info_get_uri = {
    .uri = "/api/v1/pid/info",
    .method = HTTP_GET,
    .handler = pid_info_get_handler,
};


static esp_err_t pid_params_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char buf[1024];
    int received = 0;
    if (total_len >= 10000) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    float p = (float)cJSON_GetObjectItem(root, "p")->valuedouble;
    float I = (float)cJSON_GetObjectItem(root, "i")->valuedouble;
    float D = (float)cJSON_GetObjectItem(root, "d")->valuedouble;
    pid_conf.kp = p;
    pid_conf.ki = I;
    pid_conf.kd = D;
    pid_conf.cumsum_error = 0;
    pid_conf.previous_error = 0;
    ESP_LOGI("REST", "kp:%f, ki:%f, kd:%f", p, I, D);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

httpd_uri_t pid_params_uri = {
    .uri = "/api/v1/pid/change_params",
    .method = HTTP_POST,
    .handler = pid_params_handler,
};

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    // int *v = (int *)event_data;
    ESP_LOGI("STAN", "connect success!!!: beigin start server....");
    server_setup(&server_config);
}

void app_main(void)
{
    server_config.uri[0] = post_uri;
    server_config.uri[1] = pid_info_get_uri;

    WIFICONF wifi_conf = {
        .name = "Stan",
        .password = "han112358",
        .handler = &connect_handler,
    };
    wifi_init();
    wifi_connect(&wifi_conf);
    // 初始化串口
    // uart_init(&uart_conf);

    // 初始化脉冲计数器
    pcnt_conf.encoder_num = 1;
    pcnt_conf.encoders->channel_a = 4;
    pcnt_conf.encoders->channel_b = 2;
    pcnt_init(&pcnt_conf);

    // 定时器初始化 1000ms
    timer_conf.trigger_time = 100;
    timer_conf.callback = timer_work;
    timer_conf.queue = xQueueCreate(10, sizeof(element_t));;
    timer_init(&timer_conf);

    // 初始化PWM控制器
    pwm_group.nums = 1;
    pwm_group.controllers[0].gpio = 16;
    pwm_init(&pwm_group);

    // 启动PWM
    pwm_update(&pwm_group, 0, 1);

    // 创建uart发送任务
    xTaskCreate(pwm_task, "pwm_task", 2048, NULL, 10, NULL);

    // 开启pcnt
    pcnt_start(&pcnt_conf, 0);

    // 启动定时器
    timer_start(&timer_conf);

    // printf()

    // ESP_LOGI(TAG, "timer start");

    while(1) {
        vTaskDelay(1000);
    }
}
