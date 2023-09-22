#include <stdint.h>
#include "base.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"


void delayMs(uint32_t ms) {
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

void delayUs(uint32_t us) {
    vTaskDelay(us/(1000 * portTICK_PERIOD_MS));
}

void set(gpio_num_t pin, uint32_t status) {
    gpio_set_level(pin, status);
}

int get(gpio_num_t pin) {
   return gpio_get_level(pin);
}

void pin_init(gpio_num_t pin) {
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void pin_init_input(gpio_num_t pin) {
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
}