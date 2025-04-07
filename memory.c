#include "defs.h"
#include "memlayout.h"
#include "types.h"
#include <stdint.h>
#include "proc.h"
#include "riscv.h"

typedef uint8_t uint8;
typedef uint64_t uint64;

extern char stack_end[];
extern char stack_start[];
extern char text_end[];

#define HEAP_START stack_end
#define HEAP_END (uint8 *)(KERNEL_START + MEMORY_LENGTH)
#define HEAP_SIZE ((uint64)HEAP_END - (uint64)HEAP_START)
#define TEXT_SIZE ((uint64)text_end - KERNEL_START)
#define FREEPG 0x01
#define LASTPG 0x02

long total_pages;
long alloc_start;

TrapFrame trapframe;

int free_page(uint8 desc)
{
  if (desc & FREEPG)
    return 1;
  return 0;
}

int last_page(uint8 desc)
{
  if (desc & LASTPG)
    return 1;
  return 0;
}

void set_free_page_flag(uint8 *desc, uint8 flag)
{
  if (flag == FREEPG)
  {
    *desc = *desc | FREEPG;
  }
  else
  {
    *desc = *desc & ~FREEPG;
  }
}

void set_last_page_flag(uint8 *desc, uint8 flag)
{
  if (flag == LASTPG)
  { // LASTPG = 0000 0010
    *desc = *desc | LASTPG;
  }
  else
  {
    *desc = *desc & ~LASTPG;
  }
}

// Arredonda o endereço para a próxima página
// 5000 -> 8192
// 4090 -> 4096
uint64 page_round_up(uint64 addr)
{
  if (addr % PAGE_SIZE == 0)
    return addr;
  return addr + PAGE_SIZE - (addr % PAGE_SIZE);
}

void pages_init()
{
  uint8 *desc = (uint8 *)HEAP_START;
  int reserved_pages; // paginas reservadas para os descritores;
  total_pages = HEAP_SIZE / PAGE_SIZE;
  reserved_pages = total_pages / PAGE_SIZE;
  if (total_pages % PAGE_SIZE != 0)
  {
    reserved_pages++;
  }
  total_pages -= reserved_pages;
  for (int i = 0; i < total_pages; i++)
  {
    desc[i] = FREEPG;
  }
  printf("\n\n");
  printf("+-----------------------+\n");
  printf("| Total de paginas: %d |\n", reserved_pages);
  printf("+-----------------------+\n");
  // As páginas precisam ser alinhadas (multiplo de PAGE_SIZE)
  alloc_start = page_round_up((uint64)HEAP_START + reserved_pages);
}

// Allocate multiple pages
void *kalloc2(int pages)
{
  uint8 *ponteiro;
  uint8 *freePage_Desc = 0; // Descritor da primeira página livre encontrada
  int found_pages = 0;

  for (int i = 0; i < total_pages; i++)
  {
    ponteiro = HEAP_START + i * PAGE_SIZE;
    if (free_page(ponteiro[0]))
    {
      if (found_pages == 0)
      {
        freePage_Desc = ponteiro;
      }
      found_pages++;
      if (found_pages == pages)
      {
        break;
      }
    }
    else
    {
      found_pages = 0;
      freePage_Desc = 0;
    }
  }

  if (found_pages < pages)
  { // Não encontrou páginas livres suficientes
    printf("Não encontrou páginas livres suficientes\n");
    return 0;
  }

  // Marcar as páginas como alocadas
  for (int i = 0; i < pages; i++)
  {
    set_free_page_flag(freePage_Desc + i * PAGE_SIZE, !FREEPG);
  }

  int desc_pos = (uint64)freePage_Desc - (uint64)HEAP_START;
  void *allocated_address = (void *)(desc_pos * PAGE_SIZE + alloc_start);
  printf("Páginas alocadas: %d\n", pages);
  printf("Endereço da primeira página alocada: %p\n", allocated_address);
  return allocated_address;
}

void kfree(void *ptr)
{
  if (ptr == 0)
  {
    return;
  }

  uint64 addr = (uint64)ptr;
  if (addr < alloc_start || addr >= (uint64)HEAP_END)
  {
    printf("Endereço fora do intervalo do heap\n");
    return;
  }

  int desc_pos = (addr - alloc_start) / PAGE_SIZE;
  uint8 *desc = (uint8 *)(HEAP_START + desc_pos);

  if (free_page(*desc))
  {
    printf("Página já está livre: %p\n", ptr);
    return;
  }

  // Liberar todas as páginas associadas ao bloco alocado
  while (!free_page(*desc))
  {
    set_free_page_flag(desc, FREEPG);
    
    if (last_page(*desc)) // Se for a última página do bloco, parar
    {
      set_last_page_flag(desc, 0); // Remover flag de última página
      break;
    }

    desc++; // Avança para o próximo descritor de página
  }

  printf("Bloco de páginas liberado a partir de: %p\n", ptr);
}


// Copiar a string s para a memória alocada
void kstrcpy(char *s, void *ptr)
{
  if (ptr == 0 || s == 0)
  {
    printf("Ponteiro ou string inválidos\n");
    return;
  }

  char *dest = (char *)ptr;
  while (*s)
  {
    *dest = *s;
    dest++;
    s++;
  }
  *dest = '\0'; // Adiciona o terminador nulo ao final da string copiada
  printf("String copiada para a memória alocada: %s\n", (char *)ptr);
}


void stats_kernel()

{
  uint8 *desc = (uint8 *)HEAP_START;
  long free_pages = 0;
  long allocated_pages = 0;

  for (int i = 0; i < total_pages; i++)
  {
    if (free_page(desc[i]))
    {
      free_pages++;
    }
    else
    {
      allocated_pages++;
    }
  }

  printf("\n\n");
  printf("+------------------- STATUS DO SISTEMA -------------------+\n");
  printf("| Páginas livres:          %d                    \n", free_pages);
  printf("| Páginas alocadas:        %d                   \n", allocated_pages);
  printf("| Páginas totais:          %d                   \n", total_pages);
  printf("| Página inicial alocação: %p                   \n", (void*)alloc_start);
  printf("| Tamanho da heap:         %d bytes              \n", HEAP_SIZE);
  printf("| Tamanho código kernel:   %d bytes              \n", TEXT_SIZE);
  printf("| Tamanho da pilha:        %d bytes              \n", (uint64)stack_end - (uint64)stack_start);
  printf("| Tamanho dos dados:       %d bytes              \n", (uint64)HEAP_END - (uint64)stack_end);
  printf("+--------------------------------------------------------+\n\n");
}

void memory_init()
{
  pages_init();
  //copiar o endereço de trapframe para o registrador mscratch
  w_mscratch((uint64)&trapframe);
  trapframe.trap_stack = kalloc2(1); // Alocar espaço para a pilha de exceção

  printf("+------------------------------+\n");
  printf("| Início da área de alocação: %p |\n", (void *)alloc_start);
  printf("+------------------------------+\n");

  // Alocar múltiplas páginas
  void *multi_pages = kalloc2(3);
  printf("+------------------------------+\n");
  printf("| Múltiplas páginas alocadas: %p |\n", multi_pages);
  printf("+------------------------------+\n");

  // Copiar string para a memória alocada
  char *str = "Hello, World!";
  kstrcpy(str, multi_pages);  // Copiar a string para o bloco de memória alocado
  printf("+------------------------------+\n");
  printf("| String copiada para a memória alocada: %s |\n", (char *)multi_pages);
  printf("+------------------------------+\n");

  // Liberar a memória alocada
  kfree(multi_pages);
  printf("+------------------------------+\n");
  printf("| Página liberada: %p |\n", multi_pages);
  printf("+------------------------------+\n");
}
