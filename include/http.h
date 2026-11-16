#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <sys/types.h>

// Limites independientes
#define MAX_REQUEST_LINE 300   // largo maximo de "GET /uri HTTP/1.1"
#define MAX_URI          256   // largo maximo de la URI
#define MAX_HEADER_LINE  256   // largo maximo de cada linea de encabezado
#define MAX_HEADERS      16    // cantidad maxima de encabezados

typedef struct {
    char metodo[8];                 //para almacenar GET
    char uri[MAX_URI + 1];          //para almacenar /index.html
    char version[16];               //para almacenar HTTP/1.1
    char host[256];                 //para almacenar localhost:8080
    char connection[32];            //para almacenar close o keep-alive
    char user_agent[256];           //para almacenar curl/8.0
} http_request_t;

int parsing_http_request(const char *request, ssize_t request_length, http_request_t *parsed_request);

#endif
