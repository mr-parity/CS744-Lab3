#include "kernel/types.h"
#include "user/user.h"

/* Global variables */
int global_init = 100;
const int global_const = 500;

#define PGSIZE 4096 // bytes per page
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
#define TRAMPOLINE (MAXVA - PGSIZE)

int main(int argc, char *argv[])
{
  int stack_var = 42; // Stack variable

  char *heap_ptr = sbrk(4096); // Heap allocation

  uint64 text_va = (uint64)&main;
  uint64 stack_va = (uint64)&stack_var;
  uint64 heap_va = (uint64)heap_ptr;
  uint64 global_va = (uint64)&global_init;
  uint64 const_va = (uint64)&global_const;
  uint64 tramp_va = TRAMPOLINE;

  // ========= TEXT (Code Segment) =========
  getPTEFlags(text_va);

  // ========= STACK Segment =========
  getPTEFlags(stack_va);

  // ========= HEAP Segment =========
  getPTEFlags(heap_va);

  // ========= Global Variable =========
  getPTEFlags(global_va);

  // ========= Constant Variable =========
  getPTEFlags(const_va);

  // ========= TRAMPOLINE Segment =========
  getPTEFlags(tramp_va);

  exit(0);
}
