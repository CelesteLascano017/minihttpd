#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <errno.h>

char buffer [1025];

int main(){

    int errexit (const char *format, ...);

    struct addrinfo hints;
    struct addrinfo *res = NULL;
    
    //Almacenar la direción del cliente que se conecta al servidor
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);

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

    //Accept acepta solicitudes de conexión, pero se bloquea hasta que un cliente se conecte
    int client_sd = accept(sd, (struct sockaddr *)&client_addr, &client_addr_len);
    
    if(client_sd<0){
        errexit("No se pudo aceptar la conexión: %s\n", strerror(errno));
    }else{
        printf("Cliente conecatado: %d\n", client_sd);
    }

    ssize_t rd = read(client_sd, buffer, sizeof(buffer));

    if(rd<0){
        errexit("No se pudo leer del socket: %s\n", strerror(errno));
    }else if (rd == 0){
        printf("El cliente cerró la conexión\n");
    }else if(rd>0){
        buffer[rd] = '\0';
        printf("Se recibieron %zd bytes: %s\n", rd, buffer);
    }

    freeaddrinfo(res);
}