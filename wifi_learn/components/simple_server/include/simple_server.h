#include <stdio.h>
#include <esp_http_server.h>

#include "esp_log.h"
#include "esp_vfs.h"

#include "esp_vfs_semihost.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
// #include "sdmmc_cmd.h"
#include "nvs_flash.h"

typedef struct {
    uint16_t port;
    httpd_handle_t server;
    httpd_uri_t uri[5];
    uint8_t uri_num;
} SERVERCONF;

void __init_fs();

void server_setup(SERVERCONF *conf);

