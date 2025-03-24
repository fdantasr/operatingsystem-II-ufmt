#include "types.h"
#include "defs.h"
#include <stdarg.h> // para funções com número variável de argumentos

static char *digits = "0123456789abcdef"; // Digitos para ASCII, suportando até base 16

void printlng(long val, int base){
    long l;
    char s[66];
    int i = 0; 

    if (val < 0) {
        l = -val;
    } else {
        l = val;
    }

    if (l == 0) {
        s[i++] = '0'; // Tratamento para zero
    }

    while(l != 0) {
        s[i++] = digits[l % base];  // usa a base fornecida para calcular o dígito
        l = l / base;
    }

    if (val < 0) {
        s[i++] = '-';
    }
    if (base == 16){
        s[i++] = 'x';
        s[i++] = '0';
    }

    if (base  == 2){
        s[i++] = 'b';
        s[i++] = '0';      
    }

    // Imprimir a string em ordem inversa
    while(i > 0){
        uartputc(s[--i]);
    }
}

void printptr(void *ptr) {
    printlng((long)ptr, 16);  // Converte o endereço para hexadecimal e imprime
}


void printf(char *s, ...){
    va_list ap; // argumenos variaveis
    char *str;
    int c;

    va_start(ap, s); // inicializa a lista de armgumentos variaveis

    for(; (c = *s) != 0; s++){
        if(c != '%'){
            uartputc(c);
            continue;
        }
        c = *++s; // proximo caracter depois de  %
        if (c == 0) {
            break;  // string mal formada. Ex: prinf("Hello, %")
        }
        switch(c) {
            case '%':
                uartputc('%');
                break;

            case 'd':   // imprimir inteiro
                printlng(va_arg(ap, int),10);
                break;

            case 'x':
                printlng(va_arg(ap, int), 16);
                break;

            case 'l':
                printlng(va_arg(ap, long), 10);
                break;

            case 's':
                str = va_arg(ap,char*);
                while(*str){
                    uartputc(*str++);
                }
                break;

            case 'p':
                uartputc('0');
                uartputc('x');
                printptr(va_arg(ap, void *));
                break;

            default:
                uartputc(c);
                break;
        }

    }
    
    va_end(ap);
}


