#pragma once
#ifndef STAN_SPI_DRIVER_H
#define STAN_SPI_DRIVER_H

#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

// 发送数据前的callback, true代表发送命令，false代表发送数据
typedef void (*pre_send_callback)(bool, gpio_num_t);

// SPI引脚定义
typedef struct {
    gpio_num_t    cs;
    // gpio_num_t reset;
    gpio_num_t   clk;
    gpio_num_t  mosi;
} spi_pin_t;

// SPI额外引脚定义
typedef struct {
    gpio_num_t    reset;
    gpio_num_t    dc;
    gpio_num_t   busy;
} spi_other_pin_t;



// spi结构体
typedef struct {
    // 公共参数，需要手动设置
    spi_pin_t    pin;
    spi_other_pin_t other_pin;
    uint8_t     mode;
    pre_send_callback callback;
    bool         cs_active_is_low;
    // 内部参数，无需手动设置
    uint32_t     cs_active;
    uint32_t     cs_passive;
    uint32_t     clk_status0;
    uint32_t     clk_status1;
} spi_t;

// 初始化命令结构
typedef struct {
    uint8_t cmd;
    uint8_t params[16];
    uint8_t params_len; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} spi_init_cmd_t;

// ms延时函数
void delay_ms(uint32_t ms);
// 微秒延时函数
void delay_us(uint32_t us);
// 设置引脚电平状态
void set(gpio_num_t pin, uint32_t status);
// 获取引脚电平状态
int get(gpio_num_t pin);
// 引脚初始化
void pin_init(gpio_num_t pin, gpio_mode_t mode);
// spi初始化
void spi_init(spi_t *spi);


// 发送1字节数据
void write_data(char data, spi_t *spi);
// 发送1字节命令
void write_cmd(char data, spi_t *spi);
// 发送2字节数据
void write_data_u16(uint16_t data, spi_t *spi);
// 写数据
void __write_char(char data, spi_t *spi);
// 执行SPI初始化命令序列
void execute_init_seq(const spi_init_cmd_t *cmds, spi_t *spi);

#endif