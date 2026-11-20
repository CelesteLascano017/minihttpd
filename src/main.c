#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/epoll.h>
#include <limits.h>
#include "server.h"
#include "http.h"
#include "files.h"
#include "mime.h"

#define MAX_EVENTS 64
#define BUFFER_SIZE 1024

//Estructura que representa el estado de una conexión
typedef struct{
    int fd;
    char request[BUFFER_SIZE];
    size_t request_size;
    http_request_t parsed_request;
    // Para construir dinánimcamente las cabeceras
    const response[512];
    size_t response_size;
    size_t response_sent;
    int file_fd;
    size_t file_size;
    size_t file_sent;

}client_state_t;

static void cerrar_cliente(int epdf, client_state_t *client){
    epoll_ctl(epdf, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);

    if(client->file_fd != -1){
        close(client-> file_fd);
    }

    free(client);
}

int main(){

    int sd = iniciar_servidor("8080");

    char ruta_archivo[PATH_MAX];

    if(sd<0){
        fprintf(stderr, "Error al iniciar el servidor\n");
        return EXIT_FAILURE;
    }

    int epdf = epoll_create1(0);

    if (epdf == -1){
        perror("epoll_create1");
        close(sd);
        return EXIT_FAILURE;
    }

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    event.events = EPOLLIN;
    event.data.ptr = NULL;

    if(epoll_ctl(epdf, EPOLL_CTL_ADD,sd, &event)==-1){
        perror("epoll_ctl ADD server");
        close(epdf);
        close(sd);
        return EXIT_FAILURE;
    }

    while(1){

        int ready = epoll_wait(epdf, events, MAX_EVENTS, -1);

        if(ready==-1){
            if(errno == EINTR){
                continue;
            }
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < ready; i++ ){

            /*Evento para el socket de escucha, sd*/
            if(events[i].data.ptr == NULL){

                while(1){

                    int client_sd = acceptar_cliente(sd);

                    if(client_sd == -1){
                        break;
                    }

                    client_state_t *client = calloc(1, sizeof(client_state_t));

                    if(client == NULL){
                        close(client_sd);
                        continue;
                    }

                    client->fd = client_sd;
                    client->file_fd = -1;

                    struct epoll_event client_event;
                    client_event.events = EPOLLIN | EPOLLRDHUP;
                    client_event.data.ptr = client;

                    if(epoll_ctl(epdf, EPOLL_CTL_ADD, client_sd, &client_event)==-1){
                        perror("epoll_ctl ADD client");
                        close(client_sd);
                        free(client);
                    }
                }

                continue;
            }

            /*Evento de un cliente*/
            client_state_t *client = events[i].data.ptr;

            if(events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP )){
                cerrar_cliente(epdf, client);
                continue;
            }

            /*Evento cuando el cliente tiene datos disponibles*/
            if(events[i].events & EPOLLIN){

                int request_complete = 0;

                while(1){
                    
                    ssize_t received = leer_solicitud_cliente(client->fd, client->request + client->request_size, BUFFER_SIZE - client->request_size - 1);
                    
                    if(received > 0){

                        client->request_size += (size_t)received;
                        client->request[client->request_size] = '\0';

                        if(strstr(client->request, "\r\n\r\n") != NULL){
                            request_complete = 1;
                            break;
                        }

                        if(client->request_size >= BUFFER_SIZE - 1){
                            request_complete = 1;
                            break;
                        }

                        continue;

                    }

                    if(received==0){
                        cerrar_cliente(epdf, client);
                        client = NULL;
                        break;
                    }

                    if (errno == EINTR){
                        continue;
                    }

                    if(errno== EAGAIN || errno == EWOULDBLOCK){
                        break;
                    }

                    perror("read");
                    cerrar_cliente(epdf, client);
                    client = NULL;
                    break;
                }

                if (client== NULL){
                    continue;
                }

                if(request_complete){
                    if(client->response == NULL){
                        if(preparar_respuesta(client->request, client->request_size,&client->parsed_request,&client->response,&client->response_size) == -1){
                            cerrar_cliente(epdf, client);
                            continue;
                        }
                    }

                    client->response_sent = 0;

                    struct epoll_event write_event;
                    write_event.events = EPOLLOUT | EPOLLRDHUP;
                    write_event.data.ptr = client;

                    if(epoll_ctl(epdf, EPOLL_CTL_MOD, client->fd, &write_event)==-1){
                        perror("epoll_ctl MOD");
                        cerrar_cliente(epdf, client);
                    }
                }
            }

            /*El cliente esta listo para escribir*/

            else if(events[i].events & EPOLLOUT){
                while(client->response_sent < client->response_size){

                    ssize_t sent = enviar_respuesta_cliente(
                        client->fd,
                        client->response + client->response_sent,
                        client->response_size - client->response_sent
                    );

                    if(sent > 0){
                        client->response_sent += (size_t)sent;
                        continue;
                    }

                    if(sent == -1 && errno == EINTR){
                        continue;
                    }

                    if(sent==-1 && (errno == EAGAIN || errno == EWOULDBLOCK)){
                        break;
                    }

                    cerrar_cliente(epdf, client);
                    client = NULL;
                    break;
                }

                if(client != NULL && client->response_sent == client->response_size){
                    cerrar_cliente(epdf, client);
                }
            }
        }
    }
    
    close(epdf);
    close(sd);

    return EXIT_SUCCESS;
}