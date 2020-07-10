#include "benzaiten.h"
#include "Omoikane.h"

int main() {
    struct b_socket http_listener = b_http_create_listener(3);
    b_socket_val req_sock;

    b_http_accept_http_request(&http_listener, &req_sock);

    char *request_buffer;
    request_buffer = b_http_read_request(&req_sock, 0);
    struct o_http_request http_request = o_http_request_parse(request_buffer);
    free(request_buffer);

    struct o_http_header *headers = (struct o_http_header *) calloc(0, sizeof(struct o_http_header));
    uint8_t header_count = 0;
    headers = o_http_header_append(headers, &header_count, "Connection", "Closed");
    char *response = o_http_response_create(200, "OK", headers, header_count, "");

    b_http_request_respond(&req_sock, response, strlen(response));

    o_http_request_free(&http_request);
    free(response);
    free(headers);
}

