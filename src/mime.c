#include <string.h>
#include <strings.h>
#include "mime.h"

const char *obtener_tipo_mime(const char *ruta) {
    if (ruta == NULL)
        return "application/octet-stream";

    const char *punto_extension = strrchr(ruta, '.');

    if (punto_extension == NULL){
        return "application/octet-stream";
    }

    const char *ultimo_slash = strrchr(ruta, '/');

    if (ultimo_slash != NULL && punto_extension < ultimo_slash){
        return "application/octet-stream";
    }

    if (strcasecmp(punto_extension, ".html") == 0 || strcasecmp(punto_extension, ".htm") == 0){
            return "text/html";
        }

    if (strcasecmp(punto_extension, ".css") == 0){
        return "text/css";
    }

    if (strcasecmp(punto_extension, ".js") == 0){
        return "application/javascript";
    }

    if (strcasecmp(punto_extension, ".json") == 0){
        return "application/json";
    }

    if (strcasecmp(punto_extension, ".txt") == 0){
        return "text/plain";
    }

    if (strcasecmp(punto_extension, ".png") == 0){
        return "image/png";
    }

    if (strcasecmp(punto_extension, ".jpg") == 0 || strcasecmp(punto_extension, ".jpeg") == 0){
        return "image/jpeg";
    }

    if (strcasecmp(punto_extension, ".gif") == 0){
        return "image/gif";
    }

    if (strcasecmp(punto_extension, ".svg") == 0){
        return "image/svg+xml";
    }

    if (strcasecmp(punto_extension, ".ico") == 0){
        return "image/x-icon";
    }

    return "application/octet-stream";
}
