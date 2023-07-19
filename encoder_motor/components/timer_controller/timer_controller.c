#include <stdio.h>
#include "timer_controller.h"

void timer_init(timerConf *conf) {
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    gptimer_new_timer(&timer_config, &conf->timer);
    gptimer_event_callbacks_t cbs = {
        .on_alarm = conf->callback,
    };
    gptimer_register_event_callbacks(conf->timer, &cbs, conf->queue);
    gptimer_enable(conf->timer);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000 * conf->trigger_time, // period = 1s
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(conf->timer, &alarm_config);
}

void timer_start(timerConf *conf) {
    gptimer_start(conf->timer);
}
