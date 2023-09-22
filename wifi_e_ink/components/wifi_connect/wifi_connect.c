#include <stdio.h>
#include "wifi_connect.h"

uint8_t __retry_times = 0;

static void __event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (__retry_times < __MAX_RETRY_TIMES) {
            esp_wifi_connect();
            __retry_times++;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        __retry_times = 0;
    }
}

void wifi_init() {
    // wifi依赖nvs存储
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }

    esp_netif_init();
 
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
 
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &__event_handler, NULL, &instance_any_id);
}

void wifi_connect(WIFICONF *conf) {
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, conf->handler, conf->args);
    wifi_config_t wifi_config = {
        .sta = {
	        .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    memcpy(&wifi_config.sta.ssid, &conf->name, __MAX_NAME_LEN);
    memcpy(&wifi_config.sta.password, &conf->password, __MAX_PSW_LEN);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}
