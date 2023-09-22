#ifndef _STAN_LCD_DRIVER_
#define _STAN_LCD_DRIVER_

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lvgl/lvgl.h"
#include "driver/spi_master.h"
#include "esp_log.h"


#define PIN_MISO          -1
#define PIN_MOSI          2
#define PIN_CLK           15
#define PIN_CS            18
#define PIN_DC            16
#define PIN_RST           4 
#define PIN_BLK           17

#define PIN_TOUCH_MISO    5
#define PIN_TOUCH_MOSI    PIN_MOSI
#define PIN_TOUCH_CLK     PIN_CLK
#define PIN_TOUCH_CS      19
#define PIN_TOUCH_DC      PIN_DC
#define PIN_TOUCH_PEN     21

#define PARALLEL_LINES    16

#define MAX_RED     255
#define MAX_GREEN   255
#define MAX_BLUE    255

#define SPI_MODE    0

#define LCD_HOST    HSPI_HOST


#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   480

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

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


typedef enum {
    OUT = 1,
    IN
} pin_mode_t;

void delay(uint32_t ms);

void none_spi_pin_init(gpio_num_t pins[], unsigned int num);

void pin_init(gpio_num_t pin, pin_mode_t mode);

void pin_set(gpio_num_t pin, uint32_t value);

void send_data(spi_device_handle_t spi, const uint8_t *data, int len);

void send_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active);


void run(void *params);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void spi_init();

void lcd_init();

void spi_before_transfer(spi_transaction_t *t);

#endif
