// Driver para interação com o dispositivo UART
// UART é um dispositivo de comunicação serial
#include "memlayout.h"
#include "types.h"

#define RHR 0 // P/ leitura de dados
#define THR 0 // P/ escrita de dados
#define IER 1 // P/ habilitar interrupções
#define FCR 2 // P/ controle de fila FIFO
#define ISR 2 // P/ leitura de status de interrupção
#define LCR 3 // P/ controle de linha
#define LSR 5 // P/ leitura de status de linha

void write_reg(uint64, char);
char read_reg(uint64);

void uart_init() {
  // Habilita FIFO
  write_reg(FCR, 0x01);
  // Habilita interrupções
  write_reg(IER, 0x01);
}

void write_reg(uint64 reg, char c) {
  uint8 *base = (uint8 *)UART;
  *(base + reg) = c;
}

char read_reg(uint64 reg) {
  uint8 *base = (uint8 *)UART;
  return *(base + reg);
}

// Escrita de caractere no terminal (via UART)
void uartputc(int c) {
  // Verificar se o caractere anterior foi enviado
  while ((read_reg(LSR) & (1 << 5)) == 0) // byte anterior não foi enviado
    ;
  write_reg(THR, c);
}

// Obter caractere do teclado (via UART)
int uartgetc() {
  int c;
  // Verificar se há caractere disponível
  c = read_reg(LSR);
  if ((c & 0x1) == 0) // não há caractere disponível
    return -1;
  else {
    c = read_reg(RHR);
    return c;
  }
}
