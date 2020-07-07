#include "../benzaiten.h"

extern struct b_socket
b_socket_create(int domain, int type, int protocol, int ssopt_level, int ssopt_optname, const int ssopt_optval) {
    struct b_socket _sock;
    _sock.sock = socket(domain, type, protocol);
    if (_sock.sock == 0) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif

        perror("could not create socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(_sock.sock, ssopt_level, ssopt_optname, &ssopt_optval, sizeof(ssopt_optval))) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif

        perror("Could not initialize socket");
        exit(EXIT_FAILURE);
    }

    _sock.address.sin_family = domain;

    return _sock;
}

void b_socket_bind(struct b_socket *_sock, const ushort *sock_port, in_addr_t s_addr) {
    _sock->address.sin_addr.s_addr = s_addr;
    _sock->address.sin_port = htons(*sock_port);
    if (bind(_sock->sock, (struct sockaddr *) &_sock->address, sizeof(_sock->address)) < 0) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif

        perror("Could not bind the socket");
        exit(EXIT_FAILURE);
    }
}

extern void b_socket_listen(struct b_socket *_sock, const int sock_max_backlog) {
    if (listen(_sock->sock, sock_max_backlog) < 0) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif

        perror("Error while listening");
        exit(EXIT_FAILURE);
    }
}

extern void b_socket_accept(struct b_socket *_sock, b_socket_val *accept_socket) {
    socklen_t address_size = sizeof(_sock->address);
    *accept_socket = accept(_sock->sock, (struct sockaddr *) &_sock->address,
                                     &address_size);
    if (*accept_socket < 0) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif
        perror("Could not accept a connection");
        exit(EXIT_FAILURE);
    }
}

extern void b_socket_connect(struct b_socket *_sock, int sin_family, const char *address, const ushort port) {
    struct sockaddr_in connected_socket_addr;
    connected_socket_addr.sin_family = sin_family;
    connected_socket_addr.sin_port = htons(port);

    if (inet_pton(sin_family, address, &connected_socket_addr.sin_addr) <= 0) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif

        perror("Invalid/Unsupported Address");
        exit(EXIT_FAILURE);
    }

    if (connect(_sock->sock, (struct sockaddr *) &connected_socket_addr, sizeof(connected_socket_addr)) < 0) {
#ifdef DEBUG_MODE
        printf("%s %i\n", __FILE__, __LINE__);
#endif

        perror("Socket could not connect");
        exit(EXIT_FAILURE);
    }
}


extern void b_socket_read(b_socket_val *sock, char buffer[], const size_t max_read_size) {
    read(*sock, buffer, max_read_size);
}

extern void b_socket_send(b_socket_val *sock, const char *message, uint message_length, int flags) {
    send(*sock, message, message_length, flags);
}

extern void b_socket_close(b_socket_val *sock) {
    close(*sock);
}
