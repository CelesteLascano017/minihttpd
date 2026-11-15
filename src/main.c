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

int main(){
    int sd = iniciar_servidor("8080");
    if(sd<0){
        fprintf(stderr, "Error al iniciar el servidor\n");
        return EXIT_FAILURE;
    }

    while(1){
        int client_sd = acceptar_cliente(sd);
        char buffer[1024];
        char response[]= "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 8\r\n\r\nHolaaaaa";
        leer_solicitud_cliente(client_sd, buffer, sizeof(buffer));
        enviar_respuesta_cliente(client_sd, response, strlen(response));
        close(client_sd);
    }
}