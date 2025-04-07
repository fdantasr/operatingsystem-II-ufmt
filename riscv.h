#include "types.h"
#define MSTATUS_MPP_MASK (3L << 11) // modo anterior.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)    // Habilita interrupção no modo-M


static inline uint64
r_mstatus()
{
  uint64 x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void 
w_mstatus(uint64 x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

static inline void 
w_mepc(uint64 x)
{
    asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline void
w_mscratch(uint64 x) {
    asm volatile("csrw mscratch, %0" : : "r" (x));
}

static inline uint64
r_mscratch() {
    uint64 x;
    asm volatile("csrr %0, mscratch": "=r" (x));
    return x;
}

static inline void
w_sscratch(uint64 x) {
    asm volatile("csrw sscratch, %0" : : "r" (x));
}

static inline void 
w_mtvec(uint64 x)
{
  asm volatile("csrw mtvec, %0" : : "r" (x));
}

static inline uint64
r_mie()
{
  uint64 x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void 
w_mie(uint64 x)
{
  asm volatile("csrw mie, %0" : : "r" (x));
}