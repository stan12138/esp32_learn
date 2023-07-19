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

UARTCONF uart_conf = {1, 115200};
PCNTCONF pcnt_conf;
timerConf timer_conf;

typedef struct {
    int count;
} element_t;

QueueHandle_t queue = NULL;

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
    char msg[32];
    for(;;) {
        if(xQueueReceive(queue, &ele, pdMS_TO_TICKS(2000))) {
            sprintf(msg, "%d\n", ele.count);
            uart_send(&uart_conf, msg);
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

    queue = xQueueCreate(10, sizeof(element_t));

    xTaskCreate(pwm_task, "pwm_task", 2048, NULL, 10, NULL);

    timer_conf.trigger_time = 1000;
    timer_conf.callback = timer_work;
    timer_conf.queue = queue;
    timer_init(&timer_conf);

    pcnt_start(&pcnt_conf, 0);

    timer_start(&timer_conf);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
