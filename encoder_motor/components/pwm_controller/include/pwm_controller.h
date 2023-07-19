#include "driver/mcpwm_prelude.h"
#define PWM_GROUP_ID    0
#define PWM_RESOLUTION  1000000
#define PWM_PERIOD      20000
#define PWM_MAX_CONTROL_NUM  4

typedef struct {
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t operator;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;
    int gpio;
} PWMCONTOL;

typedef struct {
    PWMCONTOL controllers[PWM_MAX_CONTROL_NUM];
    int nums;
} PWMGROUP;

static inline uint32_t __value2tick(float percent)
{
    return (uint32_t) (percent * PWM_PERIOD);
}

void __pwm_init_one(PWMCONTOL *controller);

void pwm_init(PWMGROUP *group);

void pwm_update(PWMGROUP *group, int index, float value);

