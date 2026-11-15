#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>

int iniciar_servidor(const char *port);
int acceptar_cliente(int sd);
ssize_t leer_solicitud_cliente(int client_sd, char *buffer, size_t size);
ssize_t enviar_respuesta_cliente(int client_sd, const char *response, size_t size);

#endif