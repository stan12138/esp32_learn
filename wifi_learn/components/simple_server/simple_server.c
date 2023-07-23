#include "simple_server.h"


void __init_fs() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/www",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("REST", "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("REST", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("REST", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("REST", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("REST", "Partition size: total: %d, used: %d", total, used);
    }
    return ;
}

void server_setup(SERVERCONF *conf) {
    __init_fs();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.server_port = conf->port;
    httpd_start(&conf->server, &config);
    for(int index=0; index<conf->uri_num; index++) {
        httpd_register_uri_handler(conf->server, &conf->uri[index]);
    }
}