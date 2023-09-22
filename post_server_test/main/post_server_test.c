#include <string.h>
#include <esp_http_server.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
 
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_vfs.h"

#include "wifi_connect.h"
#include "simple_server.h"
#include "cJSON.h"

char buf[122880];

static esp_err_t post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    
    int received = 0;
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';
    ESP_LOG_BUFFER_HEX("STAN", buf, total_len);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

httpd_uri_t post_uri = {
    .uri = "/api/post",
    .method = HTTP_POST,
    .handler = post_handler,
};

SERVERCONF server_config = {
    .port = 8278,
    .uri_num = 1,
};

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    ESP_LOGI("STAN", "connect success!!!: beigin start server....");
    server_setup(&server_config);
}

void app_main(void)
{
    server_config.uri[0] = post_uri;
    WIFICONF wifi_conf = {
        .name = "Stan",
        .password = "han112358",
        .handler = &connect_handler,
    };
    wifi_init();
    wifi_connect(&wifi_conf);
    while(1) {
        vTaskDelay(1000);
    }
}