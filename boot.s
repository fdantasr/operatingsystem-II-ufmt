.global _start

# mhartid é o regm que contem o ID da CPU
_start:
  csrr t0, mhartid
  bnez t0, park

  la sp, stack_end
  j entry

park:
  wfi
  j park

