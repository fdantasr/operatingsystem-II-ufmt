#include "defs.h"
#include "memlayout.h"
#include "types.h"
#include <stdint.h>

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
  int reserved_pages; // pagina reservadas para os descritores;
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
  // As páginas precisam ser alinhadas (multiplo de PAGE._SIZE)
  alloc_start = page_round_up((uint64)HEAP_START + reserved_pages);

}

void* kalloc(){
  uint8 *ponteiro;
  uint8 *freePage_Desc = 0; //Descritor da página livre encontrada
  for (int i = 0; i < total_pages; i++){
    ponteiro = HEAP_START + i * PAGE_SIZE;
    if(free_page(ponteiro[0])){
      freePage_Desc = ponteiro;
      break;
    }
  }
  if(freePage_Desc == 0){ // Não encontrou página livre
    return 0;
  }
  set_free_page_flag(freePage_Desc, !FREEPG); //Seta o bit 0 para 0 e agora preciso retornar o endereço da página alocada
  int desc_pos = (uint64)freePage_Desc - (uint64)HEAP_START;
  return (void *) (desc_pos * PAGE_SIZE + alloc_start);
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

  printf("+------------------------------+\n");
  printf("| Início da área de alocação: %p |\n", (void *)alloc_start);
  printf("+------------------------------+\n");

  char *p = kalloc();

  printf("+------------------------------+\n");
  printf("| Página alocada (página1): %p |\n", p);
  printf("+------------------------------+\n");

  // Armazena "UFMT" na primeira página alocada
  p[0] = 'U';
  p[1] = 'F';
  p[2] = 'M';
  p[3] = 'T';
  p[4] = '\0'; 

  printf("| Conteúdo armazenado: %s |\n", p);
  printf("+------------------------------+\n");
}
