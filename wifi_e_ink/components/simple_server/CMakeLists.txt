idf_component_register(SRCS "simple_server.c"
                    REQUIRES esp_http_server vfs spiffs 
                    INCLUDE_DIRS "include")
