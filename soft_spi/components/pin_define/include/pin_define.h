#pragma once
#ifndef STAN_SPI_DEFINE
#define STAN_SPI_DEFINE

#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

// 发送数据前的callback, true代表发送命令，false代表发送数据
typedef void (*pre_send_callback)(bool);

// SPI引脚定义
typedef struct {
    gpio_num_t    cs;
    // gpio_num_t reset;
    gpio_num_t   clk;
    gpio_num_t  mosi;
} spi_pin_t;

// spi结构体
typedef struct {
    // 公共参数，需要手动设置
    spi_pin_t    pin;
    uint8_t     mode;
    pres_send_callback callback;
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
void spi_init();


// 发送1字节数据
void write_data(char data);
// 发送1字节命令
void write_cmd(char data);
// 发送2字节数据
void write_data_u16(uint16_t data);
// 写数据
void __write_char(char data);
// 执行SPI初始化命令序列
void execute_init_seq(spi_init_cmd_t *cmds, spi_t *spi);


DRAM_ATTR static const lcd_init_cmd_t INIT_CMDS[]={
    {0xE0, {0x0F, 0x13, 0x1D, 0x09, 0x18, 0x0A, 0x43, 0x66, 0x4F, 0x07, 0x0F, 0x0E, 0x18, 0x1A, 0x03}, 15},
    {0xE1, {0x0F, 0x1A, 0x1D, 0x04, 0x0F, 0x04, 0x31, 0x14, 0x43, 0x03, 0x0D, 0x0C, 0x26, 0x29, 0x00}, 15},
    {0xC0, {0x14, 0x0E}, 2},
    {0xC1, {0x43}, 1},
    {0xC5, {0x00, 0x36, 0x80}, 3},
    {0x36, {0x48}, 1},
    {0x3A, {0x66}, 1},
    {0xB0, {0x00}, 1},
    {0xB1, {0xB0, 0x11}, 2},
    {0xB4, {0x02}, 1},
    {0xB6, {0x02, 0x02}, 2},
    {0xE9, {0x00}, 1},
    {0xF7, {0xA9, 0x51, 0x2C, 0x82}, 4},
    {0x11, {0}, 0x80},
    {0x29, {0}, 0},
    {0x00, {0}, 0xFF}
};
#endif