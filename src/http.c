#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"


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


    if (method_length == 0 || method_length >= sizeof(parsed_request->metodo)){
        return 400;
    }

    if (uri_length == 0 || uri_length >= sizeof(parsed_request->uri)){
        return 400;
    }

    if (version_length == 0 || version_length >= sizeof(parsed_request->version)){
        return 400;
    }

    
    memcpy(parsed_request->metodo, request, method_length);
    parsed_request->metodo[method_length] = '\0';

    memcpy(parsed_request->uri, request + first_space + 1, uri_length);
    parsed_request->uri[uri_length] = '\0';

    memcpy(parsed_request->version, request + second_space + 1, version_length);
    parsed_request->version[version_length] = '\0';

    if (strcmp(parsed_request->metodo, "GET") != 0) {
        return 405;
    }
    if (strcmp(parsed_request->version, "HTTP/1.1") != 0) {
        return 400;
    }

    return 0;
}
