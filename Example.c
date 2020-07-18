#include "http_lib.h"

int main() {
    struct lsocket http_listener = http_create_listener(3);
    socket_val req_sock;

    http_accept_http_request(&http_listener, &req_sock);

    char *request_buffer;
    request_buffer = http_read_request(&req_sock, 600);
    struct http_request http_request = http_request_parse(request_buffer);

    struct http_header *headers = (struct http_header *) calloc(0, sizeof(struct http_header));
    uint8_t header_count = 0;
    headers = http_header_append(headers, &header_count, "Connection", "Closed");
    char *response = http_response_create(200, "OK", headers, header_count, "");

    http_request_respond(&req_sock, response, strlen(response));
    http_end_request(request_buffer, response, headers, &http_request);
}

