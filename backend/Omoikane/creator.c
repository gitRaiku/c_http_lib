#include "../Omoikane.h"

char *o_http_response_create(uint16_t response_status_code, char *response_status_text, struct o_http_header *headers,
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
        response = malloc(response_size);
    }
    sprintf(response, "HTTP/1.1 %ui %s\r\n", response_status_code, response_status_text);
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

extern void
o_http_header_append(struct o_http_header *headers, uint8_t *header_count, char *header_name, char *header_value) {
    headers = realloc(headers, sizeof(headers)  + sizeof(struct o_http_header));
    struct o_http_header _header = {
            *header_name,
            *header_value
    };
    headers[*header_count] = _header;
    *header_count+=1;
}
