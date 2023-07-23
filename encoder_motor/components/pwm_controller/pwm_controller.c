#include <stdio.h>
#include "pwm_controller.h"

/*
    初始化一个mcpwm控制器
*/
void __pwm_init_one(PWMCONTOL *controller) {
    mcpwm_timer_config_t timer_config = {
        .group_id = PWM_GROUP_ID,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = PWM_RESOLUTION,
        .period_ticks = PWM_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_new_timer(&timer_config, &controller->timer);

    mcpwm_operator_config_t operator_config = {
        .group_id = PWM_GROUP_ID, // operator must be in the same group to the timer
    };
    mcpwm_new_operator(&operator_config, &controller->operator);

    mcpwm_operator_connect_timer(controller->operator, controller->timer);

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    mcpwm_new_comparator(controller->operator, &comparator_config, &controller->comparator);

    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = controller->gpio,
    };
    mcpwm_new_generator(controller->operator, &generator_config, &controller->generator);

    mcpwm_comparator_set_compare_value(controller->comparator, __value2tick(0));

    // go high on counter empty
    mcpwm_generator_set_action_on_timer_event(controller->generator,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    // go low on compare threshold
    mcpwm_generator_set_action_on_compare_event(controller->generator,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, controller->comparator, MCPWM_GEN_ACTION_LOW));

    mcpwm_timer_enable(controller->timer);
    mcpwm_timer_start_stop(controller->timer, MCPWM_TIMER_START_NO_STOP);
}

/*
    初始化多个mcpwm控制器
*/
void pwm_init(PWMGROUP *group) {
    for(int index=0;index<group->nums;index++) {
        __pwm_init_one(&group->controllers[index]);
    }
}

/*
    设定pwm的占空比
*/
void pwm_update(PWMGROUP *group, int index, float value) {
    // printf("value: %f\n", value);
    if(value>1) {
        value = 1;
    }
    if(value < 0) {
        value = 0;
    }
    mcpwm_comparator_set_compare_value(group->controllers[index].comparator, __value2tick(value));
}