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
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "driver/uart.h"

static char msg_test[50];  /* 测试使用的字符串 */

/* 串口1的配置 */
const uart_config_t uart1_config = 
{
    .baud_rate = 115200,                   /* 通讯波特率 */
    .data_bits = UART_DATA_8_BITS,         /* 每一个数据是8位 */
    .parity    = UART_PARITY_DISABLE,      /* 关闭奇偶校验 */
    .stop_bits = UART_STOP_BITS_1,         /* 停止位是1位 */
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, /* 软件控流 */
    .source_clk  = UART_SCLK_APB,
};

#define PCNT_HIGH_LIMIT 10000
#define PCNT_LOW_LIMIT  -10000

#define EC11_GPIO_A 4
#define EC11_GPIO_B 2

pcnt_unit_handle_t pcnt_unit = NULL;

static bool pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    // BaseType_t high_task_wakeup;
    // QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // send event data to queue, from this interrupt callback
    // xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
    // return (high_task_wakeup == pdTRUE);
    return true;
}

QueueHandle_t queue_1 = NULL;

static void initPCNT() {
    // pcnt_unit_handle_t pcnt_unit = *pcnt_unit_ptr;
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };
    pcnt_new_unit(&unit_config, &pcnt_unit);

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 10000,
    };
    pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config);

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = EC11_GPIO_A,
        .level_gpio_num = EC11_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a);
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = EC11_GPIO_B,
        .level_gpio_num = EC11_GPIO_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b);

    pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
    pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

    int watch_points[] = {PCNT_LOW_LIMIT, PCNT_HIGH_LIMIT};
    for (size_t i = 0; i < sizeof(watch_points) / sizeof(watch_points[0]); i++) {
        pcnt_unit_add_watch_point(pcnt_unit, watch_points[i]);
    }

    pcnt_event_callbacks_t cbs = {
        .on_reach = pcnt_on_reach,
    };
    queue_1 = xQueueCreate(10, sizeof(int));
    pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue_1);
    pcnt_unit_enable(pcnt_unit);
    pcnt_unit_clear_count(pcnt_unit);
    pcnt_unit_start(pcnt_unit);
}

void get_num(pcnt_unit_handle_t t) {
    int pulse_count = 0;
    pcnt_unit_get_count(t, &pulse_count);
    printf("Pulse count: %d\n", pulse_count);
}

typedef struct {
    int count;
} pcnt_element_t;

typedef struct {
    pcnt_unit_handle_t pcnt;
    QueueHandle_t queue;
} task_element_t;

QueueHandle_t queue = NULL;

static bool IRAM_ATTR pcnt_timer_work(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    // Retrieve count value and send to queue
    int pulse_count = 0;
    pcnt_unit_get_count(pcnt_unit, &pulse_count);
    pcnt_element_t ele = {
        .count = pulse_count
    };
    pcnt_unit_clear_count(pcnt_unit);
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

static void pwm_task(void* arg)
{
    pcnt_element_t ele;
    for(;;) {
        if(xQueueReceive(queue, &ele, pdMS_TO_TICKS(2000))) {
            sprintf(msg_test, "%d\n", ele.count);
            uart_write_bytes(UART_NUM_1,msg_test,strlen(msg_test));
            printf("Timer reloaded, count=%d\n", ele.count);
        }
    }
}

void app_main(void)
{
    
    uart_param_config(UART_NUM_1,&uart1_config);
    /* 设置串口的gpio口，esp32支持gpio口动态设置，这一次先使用默认的串口gpio */
    uart_set_pin(UART_NUM_1, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /* 启动串口1 */
    uart_driver_install(UART_NUM_1, 256, 256,  0, NULL, 0);

    queue = xQueueCreate(10, sizeof(pcnt_element_t));
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
        .on_alarm = pcnt_timer_work,
    };
    task_element_t task;
    task.queue = queue;
    task.pcnt = pcnt_unit;
    gptimer_register_event_callbacks(gptimer, &cbs, queue);
    gptimer_enable(gptimer);
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000000, // period = 1s
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);
    initPCNT();
    gptimer_start(gptimer);
}
