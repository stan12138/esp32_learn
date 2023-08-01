#ifndef __STAN_SERVER
#define __STAN_SERVER
#include <stdio.h>
#include <esp_http_server.h>
#include <fcntl.h>

#include "esp_log.h"
#include "esp_vfs.h"

// #include "esp_vfs_semihost.h"
// #include "esp_vfs_fat.h"
#include "esp_spiffs.h"
// #include "sdmmc_cmd.h"
// #include "nvs_flash.h"
#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

typedef struct {
    uint16_t port;
    httpd_handle_t server;
    httpd_uri_t uri[5];
    uint8_t uri_num;
    char root_path[16];
    char index_file[24];
} SERVERCONF;

void __init_fs();

void server_setup(SERVERCONF *conf);

/* Set HTTP response content type according to file extension */
esp_err_t __set_content_type_from_file(httpd_req_t *req, const char *filepath);
esp_err_t __get_files_handler(httpd_req_t *req);

#endif