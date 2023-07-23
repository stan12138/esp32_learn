idf_component_register(SRCS "simple_server.c"
                    REQUIRES esp_http_server vfs spiffs fatfs nvs_flash 
                    INCLUDE_DIRS "include")
