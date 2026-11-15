#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <sys/types.h>

typedef struct {
    char metodo[8]; //para almacenar GET
    char uri[1024]; //para almacenar /index.html
    char version[16]; //para almacenar HTTP/1.1
} http_request_t;

int parsing_http_request(const char *request, ssize_t request_length, http_request_t *parsed_request);

#endif 