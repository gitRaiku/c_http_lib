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

/// Read from a http request

/**
 * @param request_buf a cstring containing a http request
 * @return a o_http_request struct containing the data from the request_buf
 */
struct o_http_request o_http_request_parse(char *request_buf);

/**
 * @param header_count a uint16_t in which to store the amount of headers in the request
 * @param body_length a uint16_t in which to store the length of the body
 * @param http_buf a cstring containing the http request
 */
void o_http_request_sizes(uint16_t *header_count, uint16_t *body_length, char *http_buf);

/**
 * @param header a cstring containing a header
 * @return a o_http_header struct containing the header name and value
 */
struct o_http_header o_http_header_parse(char *header);

/**
 * @param metadata a cstring containing the metadata of a http request (the first line)
 * @return a o_http_metadata struct containing the method, url and http version used
 */
struct o_http_metadata o_http_metadata_parse(char *metadata);

/**
 * @param request a o_http_request struct the will no longer be used
 */
void o_http_request_free(struct o_http_request *request);

/// Create a http response

char *o_http_response_create(struct o_http_metadata *metadata, struct o_http_header *headers, uint8_t header_count,
                             char *body);

#endif //C_WEB_BACKEND_API_OMOIKANE_H
