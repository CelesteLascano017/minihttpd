#ifndef FILES_H
#define FILES_H

#include <stddef.h>

int resolver_ruta(const char *uri, char *ruta, size_t ruta_size);

int solicitar_archivo(int *fd, const char *ruta, size_t *tam_archivo);


#endif
