#ifndef C_WEB_BACKEND_API_OMOIKANE_H
#define C_WEB_BACKEND_API_OMOIKANE_H

#include "includes.h"

struct o_http_header {
    char header_name[30];
    char header_value[50];
};

struct o_http_metadata {
    char *method;
    char *url;
    char *http_version;
};

struct o_http_request {
    struct o_http_header *headers;
    uint8_t header_count;
    struct o_http_metadata meta;
    char *body;
};

struct o_http_request o_http_parse_request(char *request_buf);
void o_http_request_sizes(uint16_t *header_count, uint16_t *body_length, char *http_buf);
struct o_http_header o_http_parse_header(char *header);
struct o_http_metadata o_http_parse_metata(char *metadata);
void o_http_free_request(struct o_http_request *request);

#endif //C_WEB_BACKEND_API_OMOIKANE_H
