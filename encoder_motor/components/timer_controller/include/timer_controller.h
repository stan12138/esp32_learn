#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"

typedef struct {
    gptimer_handle_t timer;
    gptimer_alarm_cb_t callback;
    QueueHandle_t queue;
    int trigger_time;     // ms
} timerConf;


void timer_init(timerConf *conf);

void timer_start(timerConf *conf);
