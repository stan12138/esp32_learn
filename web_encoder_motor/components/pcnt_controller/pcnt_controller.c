#include <stdio.h>
#include "pcnt_controller.h"

#define PCNT_HIGH_LIMIT 10000
#define PCNT_LOW_LIMIT  -10000

#define EC11_GPIO_A 4
#define EC11_GPIO_B 2

void _init_one_group_pcnt(pcnt_unit_handle_t *unit, hall_encoder encoder, QueueHandle_t *queue) {
    // 脉冲计数器中的数值有上下限
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };
    pcnt_new_unit(&unit_config, unit);

    // 脉冲计数器可以过滤毛刺信号
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = PCNT_MIN_WIDTH_NS,
    };
    pcnt_unit_set_glitch_filter(*unit, &filter_config);

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = encoder.channel_a,
        .level_gpio_num = encoder.channel_b,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    pcnt_new_channel(*unit, &chan_a_config, &pcnt_chan_a);
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = encoder.channel_b,
        .level_gpio_num = encoder.channel_a,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    pcnt_new_channel(*unit, &chan_b_config, &pcnt_chan_b);

    pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
    pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

    int watch_points[] = {PCNT_LOW_LIMIT, PCNT_HIGH_LIMIT};
    for (size_t i = 0; i < sizeof(watch_points) / sizeof(watch_points[0]); i++) {
        pcnt_unit_add_watch_point(*unit, watch_points[i]);
    }

    pcnt_event_callbacks_t cbs = {
        .on_reach = __pcnt_on_reach,
    };
    *queue = xQueueCreate(10, sizeof(int));
    pcnt_unit_register_event_callbacks(*unit, &cbs, *queue);
    pcnt_unit_enable(*unit);
    pcnt_unit_clear_count(*unit);
    
}

void pcnt_init(PCNTCONF *pcnt_conf) {
    int group_num = pcnt_conf->encoder_num;
    for(int index = 0; index < group_num; index++) {
        _init_one_group_pcnt(&pcnt_conf->units[index], pcnt_conf->encoders[index], &pcnt_conf->queues[index]);
    }
}

void pcnt_start(PCNTCONF *pcnt_conf, int index) {
    pcnt_unit_start(pcnt_conf->units[index]);
}

void pcnt_get_num(PCNTCONF *pcnt_conf, int index, int *res) {
    pcnt_unit_get_count(pcnt_conf->units[index], res);
    pcnt_unit_clear_count(pcnt_conf->units[index]);
}
