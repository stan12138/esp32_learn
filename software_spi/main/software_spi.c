#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "spi_driver.h"

void before_send(bool is_cmd, gpio_num_t dc) {
    if(is_cmd) {
        set(dc, 0);
    } else {
        set(dc, 1);
    }
}

// DRAM_ATTR static const spi_init_cmd_t INIT_CMDS[]={
//     {0x12, {0}, 0x80}, 
//     {0x01, {0x27, 0x01, 0x01}, 3},
//     {0x11, {0x01}, 1},
//     {0x44, {0x00, 0x0F}, 2},
//     {0x45, {0x27, 0x01, 0x00, 0x00}, 4},
//     {0x3C, {0x05}, 1},
//     {0x21, {0x00, 0x80}, 2},
//     {0x18, {0x80}, 1},
//     {0x4E, {0x00}, 1},
//     {0x4F, {0x27, 0x01}, 0x82},
//     {0x00, {0}, 0xFF}
// };
DRAM_ATTR static const spi_init_cmd_t INIT_CMDS[]={
    {0x01, {0x37, 0x00}, 2}, 
    {0x00, {0xCF, 0x08}, 2},
    {0x06, {0xc7, 0xcc, 0x28}, 3},
    {0x04, {0}, 0},
    {0x30, {0x3C}, 1},
    {0x41, {0x00}, 1},
    {0x50, {0x77}, 1},
    {0x60, {0x22}, 1},
    {0x61, {0x02, 0x80, 0x01, 0x80}, 4},
    {0x82, {0x1E}, 1},
    {0xE5, {0x03}, 1},
    {0x00, {0}, 0xFF}
};

char image_bw[4736];
char image_red[4736];

void clear_ram(uint8_t s) {
    for(int i=0; i<4736; i++) {
        if(s == 0) {
            image_bw[i] = 0x00;
            image_red[i] = 0x00;
        } else if(s == 1) {
            image_bw[i] = 0xFF;
            image_red[i] = 0x00;
        } else if(s == 2) {
            image_bw[i] = 0x00;
            image_red[i] = 0xFF;
        }
    }
}

void update(spi_t *spi) {
    write_cmd(0x22, spi);
    write_data(0xF7, spi);

    write_cmd(0x20, spi);
}

void display(spi_t *spi) {
	write_cmd(0x10, spi);
	for(int i=0; i<122880; i++) {
        write_data(0x55, spi);
    }
	write_cmd(0x12, spi);
	// for(int i=0; i<4736; i++) {
    //     write_data(0xFF, spi);
    // }
    update(spi);
}

void app_main(void)
{
    spi_t spi_fd = {
        .pin = {
            .cs = 4,
            .clk = 2,
            .mosi = 15,
        },
        .other_pin = {
            .reset = 17,
            .dc = 16,
            .busy = 5,
        },
        .mode = 0,
        .callback = before_send,
        .cs_active_is_low = true,
    };
    spi_init(&spi_fd);
    set(spi_fd.other_pin.reset, 1);
    delay_ms(200);
    set(spi_fd.other_pin.reset, 0);
    delay_ms(5);
    set(spi_fd.other_pin.reset, 1);
    delay_ms(200);
    execute_init_seq(INIT_CMDS, &spi_fd);
    // clear_ram(0);
    display(&spi_fd);

    while(true) {
        vTaskDelay(1);
    }
}
