#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include <string.h>

// #include "esp_log.h"

#define __MAX_RETRY_TIMES   5
#define __MAX_NAME_LEN      32
#define __MAX_PSW_LEN       64



typedef struct {
    uint8_t name[__MAX_NAME_LEN];
    uint8_t password[__MAX_PSW_LEN];
    esp_event_handler_t handler;
    void *args;
} WIFICONF;


void wifi_init();

void wifi_connect(WIFICONF *conf);
