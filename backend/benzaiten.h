#ifndef C_WEB_BACKEND_API_BENZAITEN_H
#define C_WEB_BACKEND_API_BENZAITEN_H

#include "includes.h"

#define DEBUG_MODE true

/// LOW LEVEL SOCKET API

typedef int b_socket_val;

struct b_socket {
    b_socket_val sock;
    struct sockaddr_in address;
};

extern struct b_socket
b_socket_create(int domain, int type, int protocol, int ssopt_level, int ssopt_optname, const int ssopt_optval); /* Create a tcp socket */
extern void b_socket_bind(struct b_socket *_sock, const ushort *sock_port, in_addr_t s_addr); /* Bind a socket to a port on all interfaces */
extern void b_socket_listen(struct b_socket *_sock, const int sock_max_backlog); /* Let a socket listen for connections */
extern void b_socket_accept(struct b_socket *_sock, b_socket_val *accept_socket); /* Accept an incoming connection on a socket */
extern void b_socket_connect(struct b_socket *_sock, int sin_family, const char *address, const ushort port); /* Will try to connect a socket to a address and port */
extern void b_socket_read(b_socket_val *sock, char buffer[], const size_t max_read_size); /* Will read from a socket data stream */
extern void b_socket_send(b_socket_val *sock, const char *message, uint message_length, int flags); /* Will send data trough a socket data stream */
extern void b_socket_close(b_socket_val *sock); /* Will close a socket */

/// HTTP REQUEST HANDLERS
extern struct b_socket b_http_create_listener(uint max_request_backlog); /* WILL TAKE UP PORT 80 */
extern void b_http_accept_http_request(struct b_socket *_http_listener_sock, b_socket_val *http_request_socket);
extern char *b_http_read_request(b_socket_val *http_request_socket);
extern void b_http_respond_to_request(b_socket_val *http_request_socket, char *response, uint response_size);

/*
 * Http request rules:
 *  Whole http request must not be more than 4096 characters
 *
 */

/**
 EXAMPLE OF TCP SOCKET SERVER:


 {
 struct b_socket sock = b_socket_create(AF_INET, SOCK_STREAM, 0, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 1);
    const ushort port = 9991;
    b_socket_bind(&sock, &port, INADDR_ANY);
    b_socket_listen(&sock, 1);
    b_socket_val client_socket;
    b_socket_accept(&sock, &client_socket);
    b_socket_send(&client_socket, "MESSAGE", 7,0);
    char buf[1024] = {0};
    b_socket_read(&client_socket, (char *) &buf, 1024);
    printf("%s\n", buf);
    b_socket_close(&sock.sock);
 }


 */

#endif //C_WEB_BACKEND_API_BENZAITEN_H
