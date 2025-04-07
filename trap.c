#include "defs.h"
#include "proc.h"

void trap(uint64 cause, TrapFrame *tf){
    printf("Ocorreu uma exceção: %d\n", cause);
}