#include "defs.h"
#include <stdarg.h>

int perimetro(int lados, ...){
    va_list ap; // ponteiro para a lista de argumentos 

    va_start(ap, lados); // inicializa a lista de argumentos 

    int perimetro = 0;
    for(int i = 0; i < lados; i++){
        perimetro += va_arg(ap,int);
    }

    return perimetro;
}