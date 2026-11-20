#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "files.h"

int resolver_ruta(const char *uri, char *ruta, size_t ruta_size){
    char candidato[PATH_MAX];
    char *ruta_publica;
    char *ruta_canonica;
    size_t ruta_publica_len;
    size_t ruta_canonica_len;
    int total_ruta_publica;

    if (uri == NULL || ruta == NULL || ruta_size == 0)
        return -1;

    if (uri[0] != '/')
        return -1;

    ruta_publica = realpath("www", NULL);

    if (ruta_publica == NULL)
        return -1;

    total_ruta_publica = snprintf(candidato, sizeof(candidato),
                                  "%s/%s", ruta_publica, uri + 1);

    if (total_ruta_publica < 0 ||
        (size_t)total_ruta_publica >= sizeof(candidato)) {
        free(ruta_publica);
        return -1;
    }

    ruta_canonica = realpath(candidato, NULL);

    if (ruta_canonica == NULL) {
        free(ruta_publica);
        return -1;
    }

    ruta_publica_len = strlen(ruta_publica);
    ruta_canonica_len = strlen(ruta_canonica);

    if (ruta_canonica_len < ruta_publica_len ||
        strncmp(ruta_canonica, ruta_publica, ruta_publica_len) != 0 ||
        (ruta_canonica_len > ruta_publica_len &&
         ruta_canonica[ruta_publica_len] != '/')) {
        free(ruta_publica);
        free(ruta_canonica);
        return -1;
    }

    if (ruta_canonica_len >= ruta_size) {
        free(ruta_publica);
        free(ruta_canonica);
        return -1;
    }

    memcpy(ruta, ruta_canonica, ruta_canonica_len + 1);

    free(ruta_publica);
    free(ruta_canonica);

    return 0;
}