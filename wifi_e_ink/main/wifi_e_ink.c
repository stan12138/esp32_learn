#include <string.h>
#include <math.h>
#include <esp_http_server.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_system.h"
 
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_vfs.h"

#include "wifi_connect.h"
#include "simple_server.h"
#include "eink_graphic.h"
#include "spi_driver.h"

memory_t MEM = {
    .width = 640,
    .height = 384,
    .len = 122880,
};

color_map_t MAP = {
    .rgb = {{0, 0, 0}, {255, 255, 255}, {255, 0, 0}},
    .mem = {0x00, 0x03, 0x04},
    .len = 3,
};

void before_send(bool is_cmd, gpio_num_t dc) {
    if(is_cmd) {
        set(dc, 0);
    } else {
        set(dc, 1);
    }
}


spi_t spi_fd = {
    .pin = {
        .cs = 4,
        .clk = 2,
        .mosi = 15,
    },
    .other_pin = {
        .reset = 17,
        .dc = 16,
        .busy = 5,
    },
    .mode = 0,
    .callback = before_send,
    .cs_active_is_low = true,
};

void update(spi_t *spi) {
    write_cmd(0x22, spi);
    write_data(0xF7, spi);

    write_cmd(0x20, spi);
}

void display(spi_t *spi) {
	write_cmd(0x10, spi);
	for(int i=0; i<MEM.len; i++) {
        write_data(MEM.mem[i], spi);
    }
	write_cmd(0x12, spi);
    update(spi);
    // ESP_LOG_BUFFER_HEX("STAN", MEM.mem, MEM.len);
}

static esp_err_t post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    
    int received = 0;
    while (cur_len < total_len) {
        received = httpd_req_recv(req, MEM.mem + cur_len, total_len);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    // buf[total_len] = '\0';
    // ESP_LOG_BUFFER_HEX("STAN", buf, total_len);
    httpd_resp_sendstr(req, "Post control value successfully");
    display(&spi_fd);
    return ESP_OK;
}

DRAM_ATTR static const spi_init_cmd_t INIT_CMDS[]={
    {0x01, {0x37, 0x00}, 2}, 
    {0x00, {0xCF, 0x08}, 2},
    {0x06, {0xc7, 0xcc, 0x28}, 3},
    {0x04, {0}, 0},
    {0x30, {0x3C}, 1},
    {0x41, {0x00}, 1},
    {0x50, {0x77}, 1},
    {0x60, {0x22}, 1},
    {0x61, {0x02, 0x80, 0x01, 0x80}, 4},
    {0x82, {0x1E}, 1},
    {0xE5, {0x03}, 1},
    {0x00, {0}, 0xFF}
};

httpd_uri_t post_uri = {
    .uri = "/api/post",
    .method = HTTP_POST,
    .handler = post_handler,
};

SERVERCONF server_config = {
    .port = 8278,
    .uri_num = 1,
};

void cal_ip(uint32_t addr) {
    char ip[4];
    ip[0] = addr & 0xff;
    ip[1] = (addr >> 8) & 0xff;
    ip[2] = (addr >> 16) & 0xff;
    ip[3] = (addr >> 24) & 0xff;

    char p1[4] = "255";
    char p2[4] = "255";
    char p3[4] = "255";
    char p4[4] = "255";
    sprintf(p1, "%u", ip[0]);
    sprintf(p2, "%u", ip[1]);
    sprintf(p3, "%u", ip[2]);
    sprintf(p4, "%u", ip[3]);
    printf("stan, %s, %s, %s, %s\n", p1, p2, p3, p4);

}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    ip_event_got_ip_t *ip = (ip_event_got_ip_t *)event_data;
    ESP_LOGI("STAN", "connect success!!!: beigin start server....");
    // char ip_address[16];
    // esp_netif_ip_info_t ip_info;
    // esp_netif_get_ip_info(IP_EVENT_STA_GOT_IP,&ip_info);
    // printf("My IP: " IPSTR "\n", IP2STR(&ip_info.ip));
    // printf(ip_address, IPSTR, IP2STR(&(ip->ip_info.ip)));
    uint32_t addr = ip->ip_info.ip.addr;
    // printf("ip: %lu\n", addr);
    cal_ip(addr);
    // ESP_LOGI("STAN", );
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

    spi_init(&spi_fd);
    set(spi_fd.other_pin.reset, 1);
    delay_ms(200);
    set(spi_fd.other_pin.reset, 0);
    delay_ms(5);
    set(spi_fd.other_pin.reset, 1);
    delay_ms(200);
    execute_init_seq(INIT_CMDS, &spi_fd);

    // display(&spi_fd);
    pixel_t red_pixel = {
        .color = {255, 0, 0},
        .x = 0,
        .y = 0
    };
    clear_screen(&MEM);
    for(int i=0; i<30; i++) {
        for(int j=0; j<30; j++) {
            red_pixel.x = i;
            red_pixel.y = j;
            set_pixle(&MEM, &red_pixel, &MAP, 0);
        }
    }
    pixel_t black_pixel = {
        .color = {0, 0, 0},
        .x = 30,
        .y = 30
    };
    // clear_screen(&MEM);
    for(int i=0; i<30; i++) {
        black_pixel.x += 1;
        black_pixel.y += 1;
        set_pixle(&MEM, &black_pixel, &MAP, 0);
    }



    display(&spi_fd);

    while(1) {
        vTaskDelay(1000);
    }
}