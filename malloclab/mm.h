#include <stdio.h>
#include <stdbool.h>

#define NORMAL_VERBOSE 1

int mm_init (void);
void *mm_malloc (size_t size);
void mm_free (void *ptr);
void *mm_realloc(void *ptr, size_t size);
void mm_checkheap(int verbose_level);

