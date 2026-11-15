#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include "errexit.h"
#include "server.h"
#include "http.h"

int main(){
    int sd = iniciar_servidor("8080");
    if(sd<0){
        fprintf(stderr, "Error al iniciar el servidor\n");
        return EXIT_FAILURE;
    }

    while(1){
        int client_sd = acceptar_cliente(sd);
        
        char buffer[1024];

        ssize_t request = leer_solicitud_cliente(client_sd, buffer, sizeof(buffer));

        if (request <=0){
            close(client_sd);
            continue;
        }

        http_request_t parsed_request;

        int result = parsing_http_request(buffer, request, &parsed_request);

        char *response_ok =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 8\r\n"
        "Connection: close\r\n\r\n"
        "Holaaaaa";

        char *response_400 =
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Length: 12\r\n"
        "Connection: close\r\n\r\n"
        "Bad Request!";

        char *response_405 =
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Allow: GET\r\n"
        "Content-Length: 18\r\n"
        "Connection: close\r\n\r\n"
        "Method Not Allowed";

        if (result == 0) {
        enviar_respuesta_cliente(
            client_sd, response_ok, strlen(response_ok)
        );
        }
        else if (result == 400) {
            enviar_respuesta_cliente(client_sd, response_400, strlen(response_400));
        }
        else if (result == 405) {
            enviar_respuesta_cliente(client_sd, response_405, strlen(response_405));
        }
        else {
            fprintf(stderr, "Internal parser error\n");
        }

        close(client_sd);
    }
}