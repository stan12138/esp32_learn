#include "simple_server.h"

void __init_fs(char *root_path) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = root_path,
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
    __init_fs(conf->root_path);
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.server_port = conf->port;
    httpd_start(&conf->server, &config);
    for(int index=0; index<conf->uri_num; index++) {
        httpd_register_uri_handler(conf->server, &conf->uri[index]);
    }
    // 注册默认的获取文件的uri handler
    httpd_uri_t get_file_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = __get_files_handler,
        .user_ctx = conf
    };
    httpd_register_uri_handler(conf->server, &get_file_uri);
}

// 根据文件名设置响应数据的content-type
esp_err_t __set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}


/* Send HTTP response with the contents of the requested file */
esp_err_t __get_files_handler(httpd_req_t *req)
{
    char filepath[150];
    SERVERCONF *conf = (SERVERCONF *)req->user_ctx;
    strlcpy(filepath, conf->root_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, conf->index_file, sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    __set_content_type_from_file(req, filepath);

    char chunk[1024];
    ssize_t read_bytes;
    do {
        read_bytes = read(fd, chunk, 1024);
        if (read_bytes == -1) {
            ESP_LOGE("REST", "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE("REST", "File sending failed!");
                httpd_resp_sendstr_chunk(req, NULL);
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    close(fd);
    ESP_LOGI("REST", "File sending complete");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}