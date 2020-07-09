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
/**
 * @param domain the domain type of the created socket
 * @param type the type of the created socket
 * @param protocol the protocol used by the created socket
 * @param ssopt_level setsockopt level
 * @param ssopt_optname setsockopt option name
 * @param ssopt_optval setsockopt option value
 * @return a newly created socket
 */
/// TODO remake the setsockopt param definitions
extern struct b_socket
b_socket_create(int domain, int type, int protocol, int ssopt_level, int ssopt_optname, const int ssopt_optval);
/**
 * @param _sock the socket that will be binded
 * @param sock_port the port on which the socket will be binded
 * @param s_addr the address on which the port will be binded
 */
extern void b_socket_bind(struct b_socket *_sock, const ushort *sock_port, in_addr_t s_addr);
/**
 * @param _sock the socket that will listen for incoming connections
 * @param sock_max_backlog the amount of unaccepted connections that will wait to be accepted at once
 */
extern void b_socket_listen(struct b_socket *_sock, const int sock_max_backlog);
/**
 * @param _sock the socket that is listening for connections
 * @param accept_socket a pointer to a b_socket_val, where the accepted socket will be stored
 */
extern void b_socket_accept(struct b_socket *_sock, b_socket_val *accept_socket);
/**
 * @param _sock the socket with which to connect to a lstener socket
 * @param sin_family the family of the socket
 * @param address the address of the listener socket
 * @param port the port of the listener socket
 */
extern void b_socket_connect(struct b_socket *_sock, int sin_family, const char *address, const ushort port);
/**
 * @param sock the socket where data will be read from
 * @param buffer the buffer in which the data will be stored
 * @param read_size the amount of bytes being read
 */
extern void b_socket_read(b_socket_val *sock, char *buffer, const size_t read_size);
/**
 * @param sock the socket with which to send the data
 * @param message the message to be sent
 * @param message_length the length of the message
 * @param flags the send flags
 */
extern void b_socket_send(b_socket_val *sock, const char *message, uint message_length, int flags);
/**
 * @param sock the socket that will be closed
 */
extern void b_socket_close(b_socket_val *sock);

/// HTTP REQUEST HANDLERS
/**
 * Will take up port 80
 * @param max_request_backlog the amount of unaccepted connections that can wait to be accepted
 * @return a socket containing the listener socket
 */
extern struct b_socket b_http_create_listener(uint max_request_backlog);
/**
 *
 * @param _http_listener_sock the listener socket with which to accept a connection (Usually from b_http_create_listener())
 * @param http_request_socket a pointer to b_socket_val where it will store the socket file descriptor
 */
extern void b_http_accept_http_request(struct b_socket *_http_listener_sock, b_socket_val *http_request_socket);
/**
 * The return vector will need to be free() to prevent memory leaks
 * @param http_request_socket the file descriptor from which to read 1500 bytes of data
 * @param read_size the amount of bytes to read from the socket stream
 * @return a pointer to a vector containing the read data with the size equal to read_size
 */
extern char *b_http_read_request(b_socket_val *http_request_socket, uint16_t read_size);
/**
 *
 * @param http_request_socket a socket file descriptor trough which to send the response
 * @param response a cstring containing the response
 * @param response_size the length of the response
 */
extern void b_http_request_respond(b_socket_val *http_request_socket, char *response, uint response_size);
/**
 * @param http_listener the socket that needs to be closed
 */
extern void b_http_close_listener(b_socket_val *http_listener);


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
