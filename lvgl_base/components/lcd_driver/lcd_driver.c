#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"

#include "../lvgl/examples/lv_examples.h"

#include "lcd_driver.h"

SemaphoreHandle_t lock;

static spi_device_handle_t spi;

#define TICK_GAP 1

static bool IRAM_ATTR lv_tick_task(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    lv_tick_inc(TICK_GAP);//应该是给lvgl提供心跳
    return (high_task_awoken == pdTRUE);
}

void run(void *params) {
    (void)params;
    lock = xSemaphoreCreateMutex();
    ESP_LOGW("STAN", "running!");
    lv_init();

    spi_init();

    static lv_disp_draw_buf_t draw_buf;
    // lv_color_t *buf1 = heap_caps_malloc(SCREEN_WIDTH * SCREEN_HEIGHT / 12, MALLOC_CAP_DMA);
    static lv_color_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 12];
    lv_disp_draw_buf_init(&draw_buf, buffer, NULL, SCREEN_WIDTH * SCREEN_HEIGHT / 12);


    static lv_disp_drv_t disp_drv;        /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
    disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/
    disp_drv.hor_res = SCREEN_WIDTH;   /*Set the horizontal resolution of the display*/
    disp_drv.ver_res = SCREEN_HEIGHT;   /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/


    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    gptimer_new_timer(&timer_config, &gptimer);
    //======
    gptimer_event_callbacks_t cbs = {
        .on_alarm = lv_tick_task,//设置回调函数
    };

    gptimer_register_event_callbacks(gptimer, &cbs, NULL);
    gptimer_set_raw_count(gptimer, 1000);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0, // counter will reload with 0 on alarm event
            .alarm_count = TICK_GAP * 1000, // 算下来应该是1ms进一次回调函数
        .flags.auto_reload_on_alarm = true, // enable auto-reload
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);
    LV_FONT_DECLARE(siyuan)
    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 80, 50);                          /*Set its size*/
    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "韩仪你好");
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &siyuan, 0);//设置字体
    lv_obj_set_style_text_color(label, lv_color_make(0xff, 0x00, 0x00), 0);//设置颜色
    // lv_example_style_11();
    // lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    // my_disp_flush(&disp_drv, &area, &c);
    // lv_example_get_started_1();
    // printf("hello\n");
    // lv_task_handler();
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
        // lv_task_handler();
        // lv_task_handler();
        // delay(20000);
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(lock, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(lock);
            // delay(20000);
        }
    }
}


void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {

    // printf("use me lalalalla\n");
    // printf("area info: %d, %d, %d, %d\n", area->x1, area->y1, area->x2, area->y2);

    uint32_t size = lv_area_get_width(area) * lv_area_get_height(area);
    // print("area info: ");

    lv_color16_t *buffer_16bit = (lv_color16_t *) color_p;
    // uint8_t mybuf[3*size];

    uint8_t *mybuf = (uint8_t *)heap_caps_malloc(3*size, MALLOC_CAP_DMA);
    uint16_t LD = 0;
    uint32_t j = 0;
    for (uint32_t i = 0; i < size; i++) {
        LD = buffer_16bit[i].full;
        // mybuf[j] = (uint8_t) (((LD & 0xF800) >> 8) | ((LD & 0x8000) >> 13));
        mybuf[j] = (LD & 0xF800) >> 8;
        j++;
        mybuf[j] = (uint8_t) ((LD & 0x07E0) >> 3);
        j++;
        // mybuf[j] = (uint8_t) (((LD & 0x001F) << 3) | ((LD & 0x0010) >> 2));
        mybuf[j] = (uint8_t) ((LD & 0x001F) << 3);
        j++;
    }
	/* Column addresses  */
	uint8_t xb[] = {
	    (uint8_t) (area->x1 >> 8) & 0xFF,
	    (uint8_t) (area->x1) & 0xFF,
	    (uint8_t) (area->x2 >> 8) & 0xFF,
	    (uint8_t) (area->x2) & 0xFF,
	};
	uint8_t yb[] = {
	    (uint8_t) (area->y1 >> 8) & 0xFF,
	    (uint8_t) (area->y1) & 0xFF,
	    (uint8_t) (area->y2 >> 8) & 0xFF,
	    (uint8_t) (area->y2) & 0xFF,
	};
    // printf("hi!!!!\n");
    // printf("x1->%d, y1->%d, x2->%d, y2->%d\n", area->x1, area->y1, area->x2, area->y2);
	/*Column addresses*/
	send_cmd(spi, 0x2A, false);
	send_data(spi, xb, 4);
    // printf("hi1222!!!!\n");
	/*Page addresses*/
	send_cmd(spi, 0x2B, false);
	send_data(spi, yb, 4);
    // printf("hi2e2e2e!!!!\n");
	/*Memory write*/
	send_cmd(spi, 0x2C, false);
    // printf("begin send data!!!!\n");
    
	send_data(spi, mybuf, size * 3);
    heap_caps_free(mybuf);
    // printf("hi2e23e32e3!!!!\n");
    lv_disp_flush_ready(disp);
    // printf("render done!!!!\n");
}


void spi_init() {

    spi_device_interface_config_t devcfg={
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=SPI_MODE,                                //SPI mode 0
        .spics_io_num=PIN_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=spi_before_transfer,  //Specify pre-transfer callback to handle D/C line
    };    

    spi_bus_config_t BUS_CONFIG = {
        .miso_io_num=PIN_MISO,
        .mosi_io_num=PIN_MOSI,
        .sclk_io_num=PIN_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=480*320*8
    };

    spi_bus_initialize(LCD_HOST, &BUS_CONFIG, SPI_DMA_CH_AUTO);

    spi_bus_add_device(LCD_HOST, &devcfg, &spi);

    gpio_num_t non_spi_pins[] = {PIN_BLK, PIN_DC, PIN_RST};
    none_spi_pin_init(non_spi_pins, 3);

    // gpio_num_t pin = PIN_MOSI;
    // gpio_reset_pin(pin);
    // gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    // pin = PIN_CLK;
    // gpio_reset_pin(pin);
    // gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_num_t pin = 19;
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    lcd_init();
}



void delay(uint32_t ms) {
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

void pin_init(gpio_num_t pin, pin_mode_t mode) {
    gpio_reset_pin(pin);
    if(mode == OUT) {
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    } else {
        gpio_set_direction(pin, GPIO_MODE_INPUT);
    }
}

void none_spi_pin_init(gpio_num_t pins[], unsigned int num) {
    gpio_config_t io_conf = {};
    uint64_t mask = 0;
    for(unsigned int i=0; i<num; i++) {
        mask = mask | (1ULL<<pins[i]);
    }
    io_conf.pin_bit_mask = mask;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);
}

void pin_set(gpio_num_t pin, uint32_t value) {
    gpio_set_level(pin, value);
}

void send_data(spi_device_handle_t spi, const uint8_t *data, int len) {
    if (len==0) return;             //no need to send anything
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

void send_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    if (keep_cs_active) {
      t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}


void lcd_init() {
    pin_set(PIN_RST, 0);
    delay(120);
    pin_set(PIN_RST, 1);
    delay(120);
    int index = 0;
    printf("begin run init cmds\n");
    while (INIT_CMDS[index].databytes!=0xff) {
        send_cmd(spi, INIT_CMDS[index].cmd, false);
        send_data(spi, INIT_CMDS[index].data, INIT_CMDS[index].databytes&0x1F);
        if (INIT_CMDS[index].databytes&0x80) {
            delay(120);
        }
        index++;
    }
    printf("init cmds end\n");
    pin_set(PIN_BLK, 1);
}

void spi_before_transfer(spi_transaction_t *t)
{
    int dc=(int)t->user;
    pin_set(PIN_DC, dc);
}