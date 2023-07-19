#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include <stdio.h>

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

QueueHandle_t queue = NULL;

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
    queue = xQueueCreate(10, sizeof(int));
    pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue);
    pcnt_unit_enable(pcnt_unit);
    pcnt_unit_clear_count(pcnt_unit);
    pcnt_unit_start(pcnt_unit);
}

void get_num(pcnt_unit_handle_t t) {
    int pulse_count = 0;
    pcnt_unit_get_count(t, &pulse_count);
    printf("Pulse count: %d\n", pulse_count);
}


void app_main(void)
{
    // pcnt_unit_config_t unit_config = {
    //     .high_limit = PCNT_HIGH_LIMIT,
    //     .low_limit = PCNT_LOW_LIMIT,
    // };
    // pcnt_new_unit(&unit_config, &pcnt_unit);

    // pcnt_glitch_filter_config_t filter_config = {
    //     .max_glitch_ns = 10000,
    // };
    // pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config);

    // pcnt_chan_config_t chan_a_config = {
    //     .edge_gpio_num = EC11_GPIO_A,
    //     .level_gpio_num = EC11_GPIO_B,
    // };
    // pcnt_channel_handle_t pcnt_chan_a = NULL;
    // pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a);
    // pcnt_chan_config_t chan_b_config = {
    //     .edge_gpio_num = EC11_GPIO_B,
    //     .level_gpio_num = EC11_GPIO_A,
    // };
    // pcnt_channel_handle_t pcnt_chan_b = NULL;
    // pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b);

    // pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    // pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    // pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
    // pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

    // int watch_points[] = {PCNT_LOW_LIMIT, PCNT_HIGH_LIMIT};
    // for (size_t i = 0; i < sizeof(watch_points) / sizeof(watch_points[0]); i++) {
    //     pcnt_unit_add_watch_point(pcnt_unit, watch_points[i]);
    // }

    // pcnt_event_callbacks_t cbs = {
    //     .on_reach = pcnt_on_reach,
    // };
    // queue = xQueueCreate(10, sizeof(int));
    // pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue);
    // pcnt_unit_enable(pcnt_unit);
    // pcnt_unit_clear_count(pcnt_unit);
    // pcnt_unit_start(pcnt_unit);
    initPCNT();
    // Report counter value
    int pulse_count = 0;
    int event_count = 0;
    while (1) {
        if (xQueueReceive(queue, &event_count, pdMS_TO_TICKS(1000))) {
            printf("Watch point event, count: %d\n", event_count);
        } else {
            // pcnt_unit_get_count(pcnt_unit, &pulse_count);
            // printf("Pulse count: %d\n", pulse_count);
            get_num(pcnt_unit);
        }
    }
}
