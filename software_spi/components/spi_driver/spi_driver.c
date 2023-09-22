#include <stdio.h>
#include "spi_driver.h"

#define CS  17   //片选
#define RS  4    //RES 复位
#define DC  16    //data command
#define SDO 2   //SDA MOSI
#define CLK 15   // 时钟 SCL
#define BUSY 5  // busy   


// ms延时函数
void delay_ms(uint32_t ms) {
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

// 微秒延时函数
void delay_us(uint32_t us) {
    vTaskDelay(us/(1000 * portTICK_PERIOD_MS));
}

// 设置引脚电平状态
void set(gpio_num_t pin, uint32_t status) {
    gpio_set_level(pin, status);
}

// 获取引脚电平状态
int get(gpio_num_t pin) {
   return gpio_get_level(pin);
}

// 引脚初始化
void pin_init(gpio_num_t pin, gpio_mode_t mode) {
    gpio_reset_pin(pin);
    gpio_set_direction(pin, mode);
}

// 初始化引脚和电平状态，计算cs的使能电平，采样沿
void spi_init(spi_t *spi) {
    pin_init(spi->pin.cs, GPIO_MODE_OUTPUT);
    pin_init(spi->pin.clk, GPIO_MODE_OUTPUT);
    pin_init(spi->pin.mosi, GPIO_MODE_OUTPUT);
    spi->cs_active = spi->cs_active_is_low ? 0 : 1;
    spi->cs_passive = spi->cs_active_is_low? 1 : 0;
    // mode等于1/2代表是下降沿采样；0/3代表是上升沿采样
    spi->clk_status0 = (spi->mode == 1 || spi->mode == 2) ? 1 : 0;
    spi->clk_status1 = (spi->mode == 1 || spi->mode == 2)? 0 : 1;
    // 片选线需要按照设定来设置
    set(spi->pin.cs, spi->cs_passive);
    // mode等于0/1代表空闲时时钟线是低电平，mode等于2/3代表空闲时时钟线是高电平
    set(spi->pin.clk, (spi->mode == 0 || spi->mode == 1)?0:1);
    // 数据线应该设置为低电平
    set(spi->pin.mosi, 0);

    // 初始化额外引脚
    if(spi->other_pin.reset > 0) {
        pin_init(spi->other_pin.reset, GPIO_MODE_OUTPUT);
    }
    if(spi->other_pin.dc > 0) {
        pin_init(spi->other_pin.dc, GPIO_MODE_OUTPUT);
    }
    if(spi->other_pin.busy > 0) {
        pin_init(spi->other_pin.busy, GPIO_MODE_INPUT);
    }

}

// 发送1字节数据
void __write_char(char data, spi_t *spi) {
    // 使能片选
    set(spi->pin.cs, spi->cs_active);
    set(spi->pin.mosi, (data >> 7)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, (data >> 6)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, (data >> 5)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, (data >> 4)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, (data >> 3)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, (data >> 2)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, (data >> 1)&1); set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.mosi, data&1);        set(spi->pin.clk, spi->clk_status0); set(spi->pin.clk, spi->clk_status1);
    set(spi->pin.cs, spi->cs_passive);
}

// 发送1字节命令
void write_cmd(char data, spi_t *spi) {
    spi->callback(true, spi->other_pin.dc);
    __write_char(data, spi);
}

// 发送1字节数据
void write_data(char data, spi_t *spi) {
    spi->callback(false, spi->other_pin.dc);
    __write_char(data, spi);
}

// 发送2字节数据
void write_data_u16(uint16_t data, spi_t *spi) {
    spi->callback(false, spi->other_pin.dc);
    __write_char(data>>8, spi);
    __write_char(data, spi);
}

// 初始化序列
void execute_init_seq(const spi_init_cmd_t *init_cmds, spi_t *spi) {
    int index = 0;
    while (init_cmds[index].params_len!=0xff) {
        write_cmd(init_cmds[index].cmd, spi);
        for(int i=0; i<(init_cmds[index].params_len & 0x1F); i++) {
            write_data(init_cmds[index].params[i], spi);
        }
        if (init_cmds[index].params_len&0x80) {
            delay_ms(120);
        }
        index++;
    }
}
