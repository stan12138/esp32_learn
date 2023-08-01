/*
    双通道霍尔编码器脉冲计数器
    支持多组霍尔编码器，每组霍尔编码器都需要是双通道
    目前查询到的资料，esp32一共有8个脉冲计数单元，每个单元2个通道
    特性：
    1. 每次获取一次计数器数值之后，计数器会自动置0
*/

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include <stdio.h>

#define HALL_MAX_GROUPS   4      // 目前设定最多6组

#define PCNT_HIGH_LIMIT 10000    // 目标预期计数器在周期内不会超过这个值
#define PCNT_LOW_LIMIT  -10000

#define PCNT_MIN_WIDTH_NS   10000   // 滤波脉冲宽度，单位ns

// 一组双通道霍尔编码器
typedef struct {
    int channel_a;  // 通道a的gpio
    int channel_b; // 通道b的gpio
} hall_encoder;

typedef struct {
    hall_encoder encoders[HALL_MAX_GROUPS];
    pcnt_unit_handle_t units[HALL_MAX_GROUPS];
    QueueHandle_t queues[HALL_MAX_GROUPS];
    int encoder_num; // 当前编码器组数
} PCNTCONF;

// 到达观察点的行为
static bool __pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    // BaseType_t high_task_wakeup;
    // QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // send event data to queue, from this interrupt callback
    // xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
    // return (high_task_wakeup == pdTRUE);
    return true;
}

void _init_one_group_pcnt(pcnt_unit_handle_t *unit, hall_encoder encoder, QueueHandle_t *queue);

void pcnt_init(PCNTCONF *pcnt_conf);

void pcnt_start(PCNTCONF *pcnt_conf, int index);

void pcnt_get_num(PCNTCONF *pcnt_conf, int index, int *res);