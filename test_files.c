#include <stdio.h>
#include <limits.h>
#include "files.h"

int main(void)
{
    char ruta[PATH_MAX];

    if (resolver_ruta("/index.html", ruta, sizeof(ruta)) == 0)
        printf("Ruta valida: %s\n", ruta);
    else
        printf("Ruta rechazada\n");

    return 0;
}
