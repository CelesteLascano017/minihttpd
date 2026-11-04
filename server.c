#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <errno.h>

int main(){

    int errexit (const char *format, ...);

    struct addrinfo hints;
    struct addrinfo *res = NULL;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //getaddrinfo devuelve una estructura addrinfo que se usara en bind
    int status = getaddrinfo(NULL, "8000", &hints, &res);

    if (status != 0){
        fprintf(stderr, "%s\n", gai_strerror(status));
    } 

    printf("Family: %d\n", res->ai_family);
    printf("Type: %d\n", res->ai_socktype);
    printf("Protocol: %d\n", res->ai_protocol);

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    //int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sd<0){
        errexit ("No se pudo crear el socket: %s\n", strerror(errno));
    }

    //Bind recibe la información de getaddrinfo y la asocia al socket creado
    int bd = bind(sd, res->ai_addr, res-> ai_addrlen);

    if(bd<0){
        errexit("No se pudo asociar el socket: %s\n", strerror(errno));
    }

    //Listen pone el socket en modo escucha
    int ls = listen(sd, 10);

    if(ls<0){
        errexit("No se pudo poner el socket en modo escucha: %s\n", strerror(errno));
    }

    freeaddrinfo(res);
}