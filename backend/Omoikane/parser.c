#include "../Omoikane.h"

uint8_t reached_body;
uint8_t reached_value;
uint8_t passed_first_line;
/// TODO Use a single int that holds all the flags

/*
 * 1  = reached body flag
 * 2  = reached header value flag
 * 4  =
 * 8  =
 * 16 =
 * 32 =
 * 64 =
 */

struct o_http_request o_http_request_parse(char *request_buf) {
    struct o_http_request http_request = {};
    char *current_line;

    uint16_t header_count = 0, body_length = 0;
    o_http_request_sizes(&header_count, &body_length, request_buf);
    reached_body = 0;
    passed_first_line = 0;

    http_request.headers = malloc(sizeof(struct o_http_header) * header_count);
    http_request.header_count = header_count;
    uint8_t current_header = 0;
    http_request.body = malloc(sizeof(char) * body_length);

    while ((current_line = strsep(&request_buf, "\r")) != NULL) {
        if (passed_first_line == 0) {
            http_request.meta = o_http_metadata_parse(current_line);
            passed_first_line = 1;
        } else {
            if (*current_line == '\n') {
                current_line++;
            } /// Get rid of newline
            /// TODO change delimiter to include \n too

            if (*current_line == 0) {
                reached_body = 1;
                continue;
            } /// Check if line is blanck (Delimiter between headers and body)

            if (reached_body) {
                memcpy(http_request.body, current_line, strlen(current_line));
                break;
            } else {
                http_request.headers[current_header] = o_http_header_parse(current_line);
                current_header++;
            }
        }
    }

    return http_request;
}

void o_http_request_sizes(uint16_t *header_count, uint16_t *body_length, char *http_buf) {
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

struct o_http_header o_http_header_parse(char *header) {
    struct o_http_header current_header = {};
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

struct o_http_metadata o_http_metadata_parse(char *metadata) {
    struct o_http_metadata meta = {};

    char method[8] = {0};
    char url[100] = {0};
    char http_version[10] = {0};
    uint8_t mlen, ulen, hlen;

    sscanf(metadata, "%s %s %s", method, url, http_version);
    mlen = strlen(method);
    ulen = strlen(url);
    hlen = strlen(http_version);
    meta.method = malloc(sizeof(char) * mlen);
    memcpy(meta.method, method, mlen);
    meta.url = malloc(sizeof(char) * ulen);
    memcpy(meta.url, url, ulen);
    meta.http_version = malloc(sizeof(char) * hlen);
    memcpy(meta.http_version, http_version, hlen);

    return meta;
}

void o_http_request_free(struct o_http_request *request) {
    free(request->meta.http_version);
    free(request->meta.url);
    free(request->meta.method);

    free(request->body);

    free(request->headers);
}