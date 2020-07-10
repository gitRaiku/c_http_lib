#include "../benzaiten.h"

extern struct b_socket b_http_create_listener(uint max_request_backlog) {
    struct b_socket http_listener_sock = b_socket_create(AF_INET, SOCK_STREAM, 0, SOL_SOCKET,
                                                         SO_REUSEADDR | SO_REUSEPORT, 1);
    const ushort port = 80;
    b_socket_bind(&http_listener_sock, &port, INADDR_ANY);
    b_socket_listen(&http_listener_sock, max_request_backlog);
    return http_listener_sock;
}

extern void b_http_accept_http_request(struct b_socket *_http_listener_sock, b_socket_val *http_request_socket) {
    b_socket_accept(_http_listener_sock, http_request_socket);
}

extern char *b_http_read_request(b_socket_val *http_request_socket, uint16_t read_size) {
    char *buf = (char *) calloc(read_size, sizeof(char));
    b_socket_read(http_request_socket, buf, read_size);
    return buf;
}

extern void b_http_request_respond(b_socket_val *http_request_socket, char *response, uint response_size) {
    b_socket_send(http_request_socket, response, response_size);
}

extern void b_http_close_listener(b_socket_val *http_listener) {
    b_socket_close(http_listener);
}