#include "types.h"
struct trap_frame {
    uint64 ra;  // Return Address
    uint64 sp;  // Stack Pointer
    uint64 gp;  // Global Pointer
    uint64 tp;  // Thread Pointer
    uint64 t0, t1, t2; // Temporários
    uint64 s0, s1; // Saved registers
    uint64 a0, a1, a2, a3, a4, a5, a6, a7; // Argumentos de função
    uint64 s2, s3, s4, s5, s6, s7, s8, s9, s10, s11; // Saved registers adicionais
    uint64 t3, t4, t5, t6; // Temporários adicionais
    uint8* trap_stack;
    uint64 hartid;
    uint64 epc;
};


typedef struct trap_frame TrapFrame;
