#pragma once
#ifndef STAN_SPI_LCD_BASE
#define STAN_SPI_LCD_BASE

#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

void delayMs(uint32_t ms);

void set(gpio_num_t pin, uint32_t status);

void pin_init(gpio_num_t pin);

int get(gpio_num_t pin);

void pin_init_input(gpio_num_t pin);

void delayUs(uint32_t us);

#endif