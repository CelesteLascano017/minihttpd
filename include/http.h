#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <sys/types.h>
#include <stddef.h>

#define MAX_REQUEST_LINE 300   // largo de "GET /uri HTTP/1.1"
#define MAX_URI          256   // largo de la URI
#define MAX_HEADER_LINE  256   // largo de cada linea de encabezado
#define MAX_HEADERS      16    // cantidad de encabezados

typedef struct {
    char metodo[8];                 //para almacenar GET
    char uri[MAX_URI + 1];          //para almacenar /index.html
    char version[16];               //para almacenar HTTP/1.1
    char host[256];                 //para almacenar localhost:8080
    char connection[32];            //para almacenar close o keep-alive
    char user_agent[256];           //para almacenar curl/8.0
} http_request_t;

int parsing_http_request(const char *request, ssize_t request_length, http_request_t *parsed_request);

int preparar_respuesta(int codigo_estado, const char*tipo_mime, size_t contenido_size, char*respuesta, size_t request_size,  size_t *respuesta_generada);

#endif
