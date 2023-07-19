#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

static char msg_test[] = "hello,stan\n";  /* 测试使用的字符串 */

/* 串口1的配置 */
const uart_config_t uart1_config = 
{
    .baud_rate = 115200,                   /* 通讯波特率 */
    .data_bits = UART_DATA_8_BITS,         /* 每一个数据是8位 */
    .parity    = UART_PARITY_DISABLE,      /* 关闭奇偶校验 */
    .stop_bits = UART_STOP_BITS_1,         /* 停止位是1位 */
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, /* 软件控流 */
    .source_clk  = UART_SCLK_APB,
};

void app_main(void)
{
    uart_param_config(UART_NUM_1,&uart1_config);
    /* 设置串口的gpio口，esp32支持gpio口动态设置，这一次先使用默认的串口gpio */
    uart_set_pin(UART_NUM_1, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /* 启动串口1 */
    uart_driver_install(UART_NUM_1, 256, 256,  0, NULL, 0);                 
    while(1)
    {
        uart_write_bytes(UART_NUM_1,msg_test,strlen(msg_test));  /* 将字符串“hello,world"放入串口1的TX_FIFO */
        vTaskDelay(500 / portTICK_PERIOD_MS);   /* 延时300ms */
    }
}
