#include "simple_server.h"

void server_setup(SERVERCONF *conf) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.server_port = conf->port;
    httpd_start(&conf->server, &config);
    for(int index=0; index<conf->uri_num; index++) {
        httpd_register_uri_handler(conf->server, &conf->uri[index]);
    }
}
