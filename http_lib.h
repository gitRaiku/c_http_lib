#ifndef C_WEB_BACKEND_API_HTTP_LIB_H
#define C_WEB_BACKEND_API_HTTP_LIB_H

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

typedef int socket_val;

struct lsocket {
    socket_val sock;
    struct sockaddr_in address;
};

struct http_header {
    char header_name[30];
    char header_value[50];
};

struct o_http_metadata {
    char *method;
    char *url;
    char *http_version;
};

struct http_request {
    struct http_header *headers;
    uint8_t header_count;
    struct o_http_metadata meta;
    char *body;
};

/**
 * @param domain the domain type of the created lsocket
 * @param type the type of the created lsocket
 * @param protocol the protocol used by the created lsocket
 * @param ssopt_level setsockopt level
 * @param ssopt_optname setsockopt option name
 * @param ssopt_optval setsockopt option value
 * @return a newly created lsocket
 */
/// TODO remake the setsockopt param definitions
extern struct lsocket
socket_create(int domain, int type, int protocol, int ssopt_level, int ssopt_optname, const int ssopt_optval);

/**
 * @param _sock the lsocket that will be binded
 * @param sock_port the port on which the lsocket will be binded
 * @param s_addr the address on which the port will be binded
 */
extern void socket_bind(struct lsocket *_sock, const ushort *sock_port, in_addr_t s_addr);

/**
 * @param _sock the lsocket that will listen for incoming connections
 * @param sock_max_backlog the amount of unaccepted connections that will wait to be accepted at once
 */
extern void socket_listen(struct lsocket *_sock, const int sock_max_backlog);

/**
 * @param _sock the lsocket that is listening for connections
 * @param accept_socket a pointer to a socket_val, where the accepted lsocket will be stored
 */
extern void socket_accept(struct lsocket *_sock, socket_val *accept_socket);

/**
 * @param _sock the lsocket with which to connect to a lstener lsocket
 * @param sin_family the family of the lsocket
 * @param address the address of the listener lsocket
 * @param port the port of the listener lsocket
 */
extern void socket_connect(struct lsocket *_sock, int sin_family, const char *address, const ushort port);

/**
 * @param sock the lsocket where data will be read from
 * @param buffer the buffer in which the data will be stored
 * @param read_size the amount of bytes being read
 */
extern void socket_read(socket_val *sock, char *buffer, const size_t read_size);

/**
 * @param sock the lsocket with which to send the data
 * @param message the message to be sent
 * @param message_length the length of the message
 * @param flags the send flags
 */
extern void socket_send(socket_val *sock, const char *message, uint message_length);

/**
 * @param sock the lsocket that will be closed
 */
extern void socket_close(socket_val *sock);

/// HTTP REQUEST HANDLERS
/**
 * Will take up port 80
 * @param max_request_backlog the amount of unaccepted connections that can wait to be accepted
 * @return a lsocket containing the listener lsocket
 */
extern struct lsocket http_create_listener(uint max_request_backlog);

/**
 *
 * @param _http_listener_sock the listener lsocket with which to accept a connection (Usually from http_create_listener())
 * @param http_request_socket a pointer to socket_val where it will store the lsocket file descriptor
 */
extern void http_accept_http_request(struct lsocket *_http_listener_sock, socket_val *http_request_socket);

/**
 * The return vector will need to be free() to prevent memory leaks
 * @param http_request_socket the file descriptor from which to read 1500 bytes of data
 * @param read_size the amount of bytes to read from the lsocket stream
 * @return a pointer to a vector containing the read data with the size equal to read_size
 */
extern char *http_read_request(socket_val *http_request_socket, uint16_t read_size);

/**
 *
 * @param http_request_socket a lsocket file descriptor trough which to send the response
 * @param response a cstring containing the response
 * @param response_size the length of the response
 */
extern void http_request_respond(socket_val *http_request_socket, char *response, uint response_size);

/**
 * @param http_listener the lsocket that needs to be closed
 */
extern void http_close_listener(socket_val *http_listener);

/**
 * Free all the memory used in a http request
 * @param request_buffer A pointer to the buffer you used to hold the request (NULL if you wish to keep the buffer)
 * @param response A pointer to the cstring you sent as the response
 * @param headers A pointer to all the headers used to create the response (NULL if you had no headers)
 * @param http_request A pointer to the parsed http_request
 */
extern void
http_end_request(char *request_buffer, char *response, struct http_header *headers, struct http_request *http_request);

/// Read from a http request

/**
 * @param request_buf a cstring containing a http request
 * @return a http_request struct containing the data from the request_buf
 */
extern struct http_request http_request_parse(char *request_buf);

/**
 * @param header_count a uint16_t in which to store the amount of headers in the request
 * @param body_length a uint16_t in which to store the length of the body
 * @param http_buf a cstring containing the http request
 */
void http_request_sizes(uint16_t *header_count, uint16_t *body_length, char *http_buf);

/**
 * @param header a cstring containing a header
 * @return a http_header struct containing the header name and value
 */
struct http_header http_header_parse(char *header);

/**
 * @param metadata a cstring containing the metadata of a http request (the first line)
 * @return a o_http_metadata struct containing the method, url and http version used
 */
struct o_http_metadata http_metadata_parse(char *metadata);

/**
 * @param request a http_request struct the will no longer be used
 */
extern void http_request_free(struct http_request *request);

/// Create a http response

extern char *
http_response_create(uint16_t response_status_code, char *response_status_text, struct http_header *headers,
                     uint8_t header_count, char *body);

extern struct http_header *
http_header_append(struct http_header *headers, uint8_t *header_count, char *header_name, char *header_value);

/////////////////////////////////// Code ////////////////////////////////

////SOCKETS////

extern struct lsocket
socket_create(int domain, int type, int protocol, int ssopt_level, int ssopt_optname, const int ssopt_optval) {
    struct lsocket _sock;
    _sock.sock = socket(domain, type, protocol);
    if (_sock.sock == 0) {
        perror("could not create lsocket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(_sock.sock, ssopt_level, ssopt_optname, &ssopt_optval, sizeof(ssopt_optval))) {
        perror("Could not initialize lsocket");
        exit(EXIT_FAILURE);
    }

    _sock.address.sin_family = domain;

    return _sock;
}

void socket_bind(struct lsocket *_sock, const ushort *sock_port, in_addr_t s_addr) {
    _sock->address.sin_addr.s_addr = s_addr;
    _sock->address.sin_port = htons(*sock_port);
    if (bind(_sock->sock, (struct sockaddr *) &_sock->address, sizeof(_sock->address)) < 0) {
        perror("Could not bind the lsocket");
        exit(EXIT_FAILURE);
    }
}

extern void socket_listen(struct lsocket *_sock, const int sock_max_backlog) {
    if (listen(_sock->sock, sock_max_backlog) < 0) {
        perror("Error while listening");
        exit(EXIT_FAILURE);
    }
}

extern void socket_accept(struct lsocket *_sock, socket_val *accept_socket) {
    socklen_t address_size = sizeof(_sock->address);
    *accept_socket = accept(_sock->sock, (struct sockaddr *) &_sock->address,
                            &address_size);
    if (*accept_socket < 0) {
        perror("Could not accept a connection");
        exit(EXIT_FAILURE);
    }
}

extern void socket_connect(struct lsocket *_sock, int sin_family, const char *address, const ushort port) {
    struct sockaddr_in connected_socket_addr;
    connected_socket_addr.sin_family = sin_family;
    connected_socket_addr.sin_port = htons(port);

    if (inet_pton(sin_family, address, &connected_socket_addr.sin_addr) <= 0) {
        perror("Invalid/Unsupported Address");
        exit(EXIT_FAILURE);
    }

    if (connect(_sock->sock, (struct sockaddr *) &connected_socket_addr, sizeof(connected_socket_addr)) < 0) {
        perror("Socket could not connect");
        exit(EXIT_FAILURE);
    }
}

extern void socket_read(socket_val *sock, char *buffer, const size_t read_size) {
    read(*sock, buffer, read_size);
}

extern void socket_send(socket_val *sock, const char *message, uint message_length) {
    send(*sock, message, message_length, 0);
}

extern void socket_close(socket_val *sock) {
    close(*sock);
}

////HTTP////

extern struct lsocket http_create_listener(uint max_request_backlog) {
    struct lsocket http_listener_sock = socket_create(AF_INET, SOCK_STREAM, 0, SOL_SOCKET,
                                                      SO_REUSEADDR | SO_REUSEPORT, 1);
    const ushort port = 80;
    socket_bind(&http_listener_sock, &port, INADDR_ANY);
    socket_listen(&http_listener_sock, max_request_backlog);
    return http_listener_sock;
}

extern void http_accept_http_request(struct lsocket *_http_listener_sock, socket_val *http_request_socket) {
    socket_accept(_http_listener_sock, http_request_socket);
}

extern char *http_read_request(socket_val *http_request_socket, uint16_t read_size) {
    char *buf = (char *) calloc(read_size, sizeof(char));
    socket_read(http_request_socket, buf, read_size);
    return buf;
}

extern void http_request_respond(socket_val *http_request_socket, char *response, uint response_size) {
    socket_send(http_request_socket, response, response_size);
}

extern void http_close_listener(socket_val *http_listener) {
    socket_close(http_listener);
}

extern void
http_end_request(char *request_buffer, char *response, struct http_header *headers, struct http_request *http_request) {
    free(request_buffer);
    free(response);
    free(headers);
    http_request_free(http_request);
}

////Parser////

uint8_t reached_body;
uint8_t reached_value;
uint8_t passed_first_line;
/// TODO Use a single int that holds all the flags

/*
 * 1  = reached body flag
 * 2  = reached header value flag
 * 4  = passed_first_line flag
 * 8  =
 * 16 =
 * 32 =
 * 64 =
 */

struct http_request http_request_parse(char *request_buf) {
    struct http_request http_request = {};
    char *current_line;

    uint16_t header_count = 0, body_length = 0;
    http_request_sizes(&header_count, &body_length, request_buf);
    reached_body = 0;
    passed_first_line = 0;
    http_request.headers = (struct http_header *) calloc(header_count, sizeof(struct http_header));
    http_request.header_count = header_count;
    uint8_t current_header = 0;
    http_request.body = calloc(body_length, sizeof(char));

    while ((current_line = strsep(&request_buf, "\r")) != NULL) {
        if (passed_first_line == 0) {
            http_request.meta = http_metadata_parse(current_line);
            passed_first_line = 1;
        } else {
            if (*current_line == '\n') {
                current_line++;
            } /// Get rid of newline

            if (*current_line == 0) {
                reached_body = 1;
                continue;
            } /// Check if line is blanck (Delimiter between headers and body)

            if (reached_body) {
                memcpy(http_request.body, current_line, strlen(current_line));
                break;
            } else {
                http_request.headers[current_header] = http_header_parse(current_line);
                current_header++;
            }
        }
    }

    return http_request;
}

void http_request_sizes(uint16_t *header_count, uint16_t *body_length, char *http_buf) {
    reached_body = 0;
    while (*http_buf != 0) {
        if (*http_buf == '\r' && *(http_buf + 1) == '\n') {
            if (reached_body == 0) {
                *header_count += 1;
                http_buf++;
            }
            if (*(http_buf + 1) == '\r' && *(http_buf + 2) == '\n') {
                reached_body = 1;
                http_buf += 3;
                continue;
            }
        }
        if (reached_body == 1) {
            *body_length += 1;
        }
        http_buf++;
    }
    *header_count -= 1;
}

struct http_header http_header_parse(char *header) {
    struct http_header current_header = {};
    uint8_t name_pos = 0, value_pos = 0;
    reached_value = 0;

    while (*header != 0) {
        if (reached_value == 0 && *header == ':' && *(header + 1) == ' ') {
            reached_value = 1;
            header += 2;
            continue;
        }
        if (reached_value) {
            current_header.header_value[value_pos] = *header;
            value_pos++;
        } else {
            current_header.header_name[name_pos] = *header;
            name_pos++;
        }
        header++;
    }

    return current_header;
}

struct o_http_metadata http_metadata_parse(char *metadata) {
    struct o_http_metadata meta = {};

    char method[8] = {0};
    char url[100] = {0};
    char http_version[10] = {0};
    uint8_t mlen, ulen, hlen;

    sscanf(metadata, "%s %s %s", method, url, http_version);
    mlen = strlen(method);
    ulen = strlen(url);
    hlen = strlen(http_version);
    meta.method = (char *) calloc(mlen, sizeof(char));
    memcpy(meta.method, method, mlen);
    meta.url = (char *) calloc(mlen, sizeof(char));
    memcpy(meta.url, url, ulen);
    meta.http_version = (char *) calloc(hlen, sizeof(char));
    memcpy(meta.http_version, http_version, hlen);

    return meta;
}

void http_request_free(struct http_request *request) {
    free(request->meta.http_version);
    free(request->meta.url);
    free(request->meta.method);

    free(request->body);

    free(request->headers);
}

////HttpCreator////

char *http_response_create(uint16_t response_status_code, char *response_status_text, struct http_header *headers,
                           uint8_t header_count, char *body) {
    /*
     HTTP/1.1 <response_status_code> <response_status_text>\r\n
     Headers\r\n
     Headers\r\n
     Headers\r\n
     \r\n
     body
     */
    uint16_t headers_size = 0;
    char *response;
    {
        uint16_t response_size = 9;
        response_size += 4 + strlen(response_status_text) + 1 + 2;
        uint8_t i;
        for (i = 0; i < header_count; ++i) {
            headers_size += strlen(headers[i].header_name) + 2 + strlen(headers[i].header_value) + 2;
        }
        response_size += headers_size;
        response_size += 2;
        response_size += strlen(body);
        response = (char *) calloc(response_size, sizeof(char));
    }
    sprintf(response, "HTTP/1.1 %u %s\r\n", response_status_code, response_status_text);
    {
        uint8_t i;
        for (i = 0; i < header_count; ++i) {
            strcat(response, headers[i].header_name);
            strcat(response, ": ");
            strcat(response, headers[i].header_value);
            strcat(response, "\r\n");
        }
    }
    strcat(response, "\r\n");
    strcat(response, body);
    return response;
}

extern struct http_header *
http_header_append(struct http_header *headers, uint8_t *header_count, char *header_name, char *header_value) {
    headers = (struct http_header *) realloc(headers, sizeof(headers) + sizeof(struct http_header));
    strcpy(headers[*header_count].header_name, header_name);
    strcpy(headers[*header_count].header_value, header_value);
    *header_count += 1;
    return headers;
}

#endif //C_WEB_BACKEND_API_HTTP_LIB_H
