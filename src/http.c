#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "http.h"

static int copiar_valor(char *destino, size_t destino_size, const char *origen, size_t length){

    if (length == 0 || length >= destino_size){
        return -1;
    }

    memcpy(destino, origen, length);
    destino[length] = '\0';
    return 0;
}

int parsing_http_request(const char *request, ssize_t request_length, http_request_t *parsed_request){

    if ( request == NULL || parsed_request == NULL || request_length <= 0 ) {
        return -1;
    }

    if(request_length > 1024){
        return 400;
    }

    memset(parsed_request, 0, sizeof(*parsed_request));
 
    ssize_t line_end = -1;

    for (ssize_t i = 0; i < request_length - 1; i++){
        if (request[i] == '\r' && request[i+1] == '\n'){
            line_end = i;
            break;
        }
    }

    if (line_end == -1){
        return 400;
    }

    if (line_end > MAX_REQUEST_LINE){
        return 400;
    }


    ssize_t first_space = -1;

    for (ssize_t i = 0; i < line_end; i++){
        if (request[i] == ' '){
            first_space = i;
            break;
        }
    }

    if (first_space == -1){
        return 400;
    }


    ssize_t second_space = -1;

    for (ssize_t i = first_space + 1; i < line_end; i++){
        if (request[i] == ' '){
            second_space = i;
            break;
        }
    }

    if (second_space == -1){
        return 400;
    }

    size_t method_length  = first_space;
    size_t uri_length     = second_space - first_space - 1;
    size_t version_length = line_end - second_space - 1;


    if (uri_length > MAX_URI){
        return 400;
    }

    if (copiar_valor(parsed_request->metodo, sizeof(parsed_request->metodo), request, method_length) != 0){
        return 400;
    }

    if (copiar_valor(parsed_request->uri, sizeof(parsed_request->uri), request + first_space + 1, uri_length) != 0){
        return 400;
    }

    if (copiar_valor(parsed_request->version, sizeof(parsed_request->version), request + second_space + 1, version_length) != 0){
        return 400;
    }

    if (parsed_request->uri[0] != '/'){
        return 400;
    }

    if (strcmp(parsed_request->metodo, "GET") != 0) {
        return 405;
    }

    if (strcmp(parsed_request->version, "HTTP/1.1") != 0) {
        return 400;
    }


    ssize_t pos = line_end + 2;    
    int header_count = 0;
    int headers_terminados = 0;

    while (pos < request_length){

        ssize_t end = -1;

        for (ssize_t i = pos; i < request_length - 1; i++){
            if (request[i] == '\r' && request[i+1] == '\n'){
                end = i;
                break;
            }
        }

        if (end == -1){
            return 400;
        }

        ssize_t header_length = end - pos;

   
        if (header_length == 0){
            headers_terminados = 1;
            break;
        }


        if (header_length > MAX_HEADER_LINE){
            return 400;
        }

        header_count++;
        if (header_count > MAX_HEADERS){
            return 400;
        }


        ssize_t colon = -1;

        for (ssize_t i = pos; i < end; i++){
            if (request[i] == ':'){
                colon = i;
                break;
            }
        }


        if (colon == -1 || colon == pos){
            return 400;
        }


        for (ssize_t i = pos; i < colon; i++){
            if (request[i] == ' ' || request[i] == '\t'){
                return 400;
            }
        }

        const char *name = request + pos;
        size_t name_length = colon - pos;


        ssize_t value_start = colon + 1;
        while (value_start < end && (request[value_start] == ' ' || request[value_start] == '\t')){
            value_start++;
        }


        ssize_t value_end = end;
        while (value_end > value_start && (request[value_end - 1] == ' ' || request[value_end - 1] == '\t')){
            value_end--;
        }

        const char *value = request + value_start;
        size_t value_length = value_end - value_start;

   
        if (name_length == 4 && strncasecmp(name, "Host", 4) == 0){

            if (parsed_request->host[0] != '\0'){
                return 400;     
            }
            if (copiar_valor(parsed_request->host, sizeof(parsed_request->host), value, value_length) != 0){
                return 400;
            }
        }
        else if (name_length == 10 && strncasecmp(name, "Connection", 10) == 0){

            if (parsed_request->connection[0] != '\0'){
                return 400;     
            }
            if (copiar_valor(parsed_request->connection, sizeof(parsed_request->connection), value, value_length) != 0){
                return 400;
            }
            if (strcasecmp(parsed_request->connection, "close") != 0 &&
                strcasecmp(parsed_request->connection, "keep-alive") != 0){
                return 400;
            }
        }
        else if (name_length == 10 && strncasecmp(name, "User-Agent", 10) == 0){

            if (copiar_valor(parsed_request->user_agent, sizeof(parsed_request->user_agent), value, value_length) != 0){
                return 400;
            }
        }


        pos = end + 2;
    }


    if (headers_terminados == 0){
        return 400;
    }


    if (parsed_request->host[0] == '\0'){
        return 400;
    }

    return 0;
}


int preparar_respuesta(const char *request, size_t request_size, http_request_t *parsed_request, const char **respuesta, size_t *respuesta_size){
    if (request == NULL || parsed_request == NULL ||respuesta == NULL || respuesta_size == NULL){
        return -1;

    }

    const char *mensaje;

    switch (codigo_estado){
        case 200:
            mensaje = "Ok";
            break;
        case 400:
            mensaje = "Bad Request";
            break;
        case 403:
            mensaje = "Forbidden";
            break;
        case 404:
            mensaje = "Not Found";
            break;
        case 405:
            mensaje = "Method Not Allowed";
            break;
        case 500:
            mensaje = "Internal Server Error";
            break;
        deafult:
            return -1;
    }

    if(tipo_mime == NULL){
        tipo_mime = "application/octet-stream"
    }

    int resultado = parsing_http_request(
        respuesta, 
        respuesta_size,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        codigo_estado,
        mensaje,
        tipo_mime,
        contenido_size    
    );

    if (resultado < 0 || (size_t)resultado >= respuesta_size){
        *respuesta_generada = 0;
        return -1;
    }
       
    *respuesta_generada = (size_t)resultado;

    return 0;
}