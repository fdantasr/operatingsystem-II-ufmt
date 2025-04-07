#include "defs.h"
#include "types.h"
#include "console.h"
#include "riscv.h"

extern void mvector(); // rotina definida em assembly
// Função puts para enviar caracteres pela UART (sem dependências de bibliotecas do sistema)
void puts(char *s)
{
  while (*s)
  { // Envia cada caractere da string
    uartputc(*s);
    s++;
  }
}

void printlng(long, int);
void printptr(void *);

void main()
{
  int c;

  uart_init(); // Inicializa a UART (assumindo que já esteja definida em outro lugar)
  // Mensagem de introdução
  // Apresentação mais bonita
  puts(CLS TOP_LEFT BLUE BOLD);
  puts("***************************************\n");
  puts("*                                     *\n");
  puts("*   SOS: Spectrum Operating System    *\n");
  puts("*     Bem-vindo ao Orbite!            *\n");
  puts("*                                     *\n");
  puts("***************************************\n");
  puts(RESET_COLOR "\n");

  stats_kernel(); // Exibe as estatísticas do kernel

  // Teste de perímetro (um exemplo genérico)
  int per = perimetro(3, 10, 20, 30); // Triângulo
  per = perimetro(4, 10, 20, 30, 40); // Quadrado
  printf("Perimetro do quadrado: %d\n", per);
  printf("Um número negativo: %d\n", -12345);

  char *ptr = "Hello, World!";
  printf("Saudacoes: %s. Seu endereco: (na memoria)");

  // Teste para número em hexadecimal
  printf("Converter decimal em hexadecimal: %x\n", 11);

  // Imprime o endereço do ponteiro
  puts("Um ponteiro: ");
  ptr = "Hello, World"; // Atualiza o ponteiro
  printptr(ptr);        // Exibe o endereço de memória do ponteiro
  puts("\n");

  // Teste para o símbolo de porcentagem
  printf("O simbolo de porcentagem %%\n");

  // Teste de número long
  long longValue = 1234567890L;
  printf("Número longo: %ld\n", longValue); // Corrigido para usar %ld

  // Teste de string
  char *str = "Teste de string!";
  printf("String: %s\n", str);

  // Teste de um ponteiro
  char *hello = "Hello, pointer!";
  printf("Endereço do ponteiro: %p\n", hello);

  // Teste de porcentagem
  printf("Porcentagem %% funcionando!\n");

  // Loop para ler caracteres e realizar ações com base neles
  for (;;)
  {
    c = uartgetc(); // Obtém um caractere via UART
    if (c == -1)
    {
      continue; // Se não houver caracteres, continua
    }

    switch (c)
    {
    case '\r': // Se pressionado Enter
      uartputc('\r');
      uartputc('\n');
      break;

    case 0x7f: // Se pressionado Backspace (DEL)
      uartputc('\b');
      uartputc(' ');
      uartputc('\b');
      break;

    case 'a':          // Se pressionado 'a'
      puts("\x1B[3B"); // ANSI para mover o cursor para baixo
      break;

    default:
      uartputc(c); // Caso contrário, apenas imprime o caractere
      break;
    }
  }
}

void entry()
{
  //Queremos executar a função main no modo de supervisor
  memory_init(); // Inicializa as páginas de memória
  w_mtvec((uint64)mvector);
  printf("Endereço do vetor de interrupção: %p\n", mvector);
  w_mepc((uint64)main); // Define o endereço de retorno após a interrupção
  uint64 x = r_mstatus();
  x = x & ~MSTATUS_MPP_MASK; // Limpa os bit MPP
  x = x | MSTATUS_MPP_S;  // configura o modo de privilégio para supervisor
  w_mstatus(x); // Atualiza o status
  asm  volatile ("mret"); // Instrução assembly para retornar  o endereço da main

}