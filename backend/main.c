#include "benzaiten.h"
#include "Omoikane.h"

int main() {
    struct b_socket http_listener = b_http_create_listener(0);
    b_socket_val req_sock;
    b_http_accept_http_request(&http_listener, &req_sock);
    char *request_buffer;
    request_buffer = b_http_read_request(&req_sock);
    struct o_http_request http_request = o_http_parse_request(request_buffer);
    char response[] = "HTTP/1.1 200 OK\n\n";
    b_http_respond_to_request(&req_sock, response, 17);
    o_http_free_request(&http_request);
}

