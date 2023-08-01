#include "driver/uart.h"
#include "driver/gpio.h"

#define UART_SENDER_TX_PIN		GPIO_NUM_1
#define UART_SENDER_RX_PIN		GPIO_NUM_3
#define UART_SENDER_TX_BUF_SIZE	256
#define UART_SENDER_RX_BUF_SIZE	256

typedef struct {
    uart_port_t port;
    int rate;
} UARTCONF;

void uart_init(UARTCONF *conf);
void uart_send(UARTCONF *conf, const char *buf);


