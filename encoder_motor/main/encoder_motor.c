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
#include "driver/gptimer.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_sleep.h"

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
    1.2, 0, 0.8, 0.015, 0.002, 0, 0, 0.1
};

typedef struct {
    int count;
} element_t;

static const char *TAG = "example";

// QueueHandle_t queue = NULL;

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
            uart_send(&uart_conf, msg);
            float value = pid_update(&pid_conf);
            sprintf(msg, "%.2f,%.2f,%.2f,%.2f\n", pid_conf.aim-0.2, pid_conf.current, value, pid_conf.previous_error);
            // ESP_LOGI(TAG, "%f", value);
            pwm_update(&pwm_group, 0, value);
        }
    }
}

void app_main(void)
{
    // 初始化串口
    uart_init(&uart_conf);

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
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
