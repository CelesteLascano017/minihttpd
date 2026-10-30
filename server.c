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

    freeaddrinfo(res);
}