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
#include <fcntl.h>


int iniciar_servidor(const char *port){


    struct addrinfo hints;
    struct addrinfo *res = NULL;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //getaddrinfo devuelve una estructura addrinfo que se usara en bind
    int status = getaddrinfo(NULL, port, &hints, &res);

    if (status != 0){
        fprintf(stderr, "%s\n", gai_strerror(status));
        return -1;
    } 

    printf("Family: %d\n", res->ai_family);
    printf("Type: %d\n", res->ai_socktype);
    printf("Protocol: %d\n", res->ai_protocol);

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    //int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sd<0){
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    //Bind recibe la información de getaddrinfo y la asocia al socket creado
    int bd = bind(sd, res->ai_addr, res-> ai_addrlen);

    if(bd<0){
        perror("bind");
        close(sd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    //Listen pone el socket en modo escucha
    int ls = listen(sd, 10);

    if(ls<0){
        perror("listen");
        close(sd);
        return -1;
    }

    /*Preparamos el socket para que sea no bloqueante antes de que se use en main.c*/
    if(configurar_no_bloqueantes(sd)==-1){
        close(sd);
        return -1;
    }

    return sd;

}

int acceptar_cliente(int sd){

    //Almacenar la direción del cliente que se conecta al servidor
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));

    socklen_t client_addr_len = sizeof(client_addr);

    // accept() obtiene una conexión pendiente.
    // Si no hay conexiones disponibles, devuelve -1 con EAGAIN o EWOULDBLOCK
    int cliente_sd = accept(sd, (struct sockaddr *)&client_addr, &client_addr_len);

    if(cliente_sd==-1){
        if(errno==EAGAIN || errno == EWOULDBLOCK){
            return -1;
        }
        perror("accept");
        return -1;
    }

    if(configurar_no_bloqueantes(cliente_sd)==-1){
        close(cliente_sd);
        return -1;
    }

    printf("Cliente conectado: %d\n", cliente_sd);

    return cliente_sd;
}

ssize_t leer_solicitud_cliente(int client_sd, char *buffer, size_t buffer_size){

    ssize_t solicitud = read(client_sd, buffer, buffer_size);
    
    return solicitud;
}

ssize_t enviar_respuesta_cliente(int client_sd, const char *response, size_t size){

    ssize_t respuesta = send(client_sd, response, size, MSG_NOSIGNAL);
   
    return respuesta;
}

int configurar_no_bloqueantes(int sd){
    //Obtener los flags actuales del descriptor
    int flags = fcntl(sd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }

    //Conservo los flags existentes y activo O_NONBLOCK
    if (fcntl(sd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}