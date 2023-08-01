#include <stdio.h>
#include <string.h> 

#include "uart_sender.h"

// 初始化
void uart_init(UARTCONF *conf) {
    const uart_config_t uart_config = {
        .baud_rate = conf->rate,                   /* 通讯波特率 */
        .data_bits = UART_DATA_8_BITS,         /* 每一个数据是8位 */
        .parity    = UART_PARITY_DISABLE,      /* 关闭奇偶校验 */
        .stop_bits = UART_STOP_BITS_1,         /* 停止位是1位 */
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, /* 软件控流 */
        .source_clk  = UART_SCLK_APB,
    };
    uart_param_config(conf->port, &uart_config);
    uart_set_pin(conf->port, UART_SENDER_TX_PIN, UART_SENDER_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /* 启动串口1 */
    uart_driver_install(conf->port, UART_SENDER_TX_BUF_SIZE, UART_SENDER_TX_BUF_SIZE,  0, NULL, 0);
}

// 发送一条数据
void uart_send(UARTCONF *conf, const char *buf) {
    uart_write_bytes(conf->port, buf, strlen(buf));
}
