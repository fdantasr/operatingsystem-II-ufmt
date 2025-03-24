/*  
Coloca a seção de texto no endereço 0x80000000 como espera
a VM `virt` (RISC-V) do Qemu
*/

OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
    . = 0x80000000;
    /* seção de código */
    .text :
    {
    *(.text)
    }
    . = ALIGN(0x1000); /* separar o texto dos dados */
    PROVIDE(text_end = .);
    /* seção de dados */
    .data :
    {
    *(.data)
    }

    .bss :
    {
        *(.bss)
        *(.bss*)
    }
    . = ALIGN(16);
    /* definir a pilha */
    PROVIDE(stack_start = .);
    PROVIDE(stack_end = stack_start + 0x4000); /* pilha de 16KB */
}
