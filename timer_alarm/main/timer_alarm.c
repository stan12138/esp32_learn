/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"

static const char *TAG = "example";

typedef struct {
    uint64_t event_count;
} example_queue_element_t;

QueueHandle_t queue = NULL;

static bool IRAM_ATTR example_timer_on_alarm_cb_v2(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    // Retrieve count value and send to queue
    example_queue_element_t ele = {
        .event_count = edata->count_value
    };
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

static void pwm_task(void* arg)
{
    example_queue_element_t ele;
    for(;;) {
        if(xQueueReceive(queue, &ele, pdMS_TO_TICKS(2000))) {
            ESP_LOGI(TAG, "Timer reloaded, count=%llu", ele.event_count);
        }
    }
}

void app_main(void)
{
    
    queue = xQueueCreate(10, sizeof(example_queue_element_t));
    if (!queue) {
        return;
    }
    xTaskCreate(pwm_task, "pwm_task", 2048, NULL, 10, NULL);
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb_v2,
    };
    gptimer_register_event_callbacks(gptimer, &cbs, queue);
    gptimer_enable(gptimer);
    gptimer_alarm_config_t alarm_config2 = {
        .reload_count = 0,
        .alarm_count = 1000000, // period = 1s
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_config2);
    gptimer_start(gptimer);
}
