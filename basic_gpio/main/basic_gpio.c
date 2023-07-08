#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

#define BLINK_GPIO  2
uint32_t status = 0;

void app_main(void)
{
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        status = !status;
        gpio_set_level(BLINK_GPIO, status);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
