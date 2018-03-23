#include <unistd.h>
#include <stdbool.h>

// you may use these functions in mm.c
void *mem_sbrk(int incr);
void mem_reset_brk(void); 
void *mem_heap_lo(void);
void *mem_heap_hi(void);
size_t mem_heapsize(void);
size_t mem_pagesize(void);

// you may also use these helper functions in mm.c
#define ALIGNMENT 16
size_t align(size_t sz);
bool is_aligned(char *p);

// these functions below are only called by the tester
void mem_init(void);               
void mem_deinit(void);

