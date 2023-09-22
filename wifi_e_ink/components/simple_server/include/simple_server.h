#ifndef __STAN_SERVER
#define __STAN_SERVER
#include <stdio.h>
#include <esp_http_server.h>
#include <fcntl.h>

#include "esp_log.h"
#include "esp_vfs.h"

#include "esp_spiffs.h"

typedef struct {
    uint16_t port;
    httpd_handle_t server;
    httpd_uri_t uri[5];
    uint8_t uri_num;
} SERVERCONF;

void server_setup(SERVERCONF *conf);

#endif