#include "benzaiten.h"
#include "Omoikane.h"

int main() {
    struct b_socket http_listener = b_http_create_listener(0);
    b_socket_val req_sock;
    b_http_accept_http_request(&http_listener, &req_sock);
    char *request_buffer;
    request_buffer = b_http_read_request(&req_sock, 0);
    printf("%s\n", request_buffer);

    struct o_http_request http_request = o_http_request_parse(request_buffer);
    free(request_buffer);
    struct o_http_header *headers = malloc(0);
    uint8_t header_count = 0;
    o_http_header_append(headers, &header_count, "Allow", "1");
    char *response = o_http_response_create(200, "OK", headers, header_count, "I have gotten your message mister");
    b_http_request_respond(&req_sock, response, strlen(response));
    o_http_request_free(&http_request);

    free(response);
    free(headers);
}

