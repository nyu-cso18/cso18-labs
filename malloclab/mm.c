/*
 * mm.c -- implement your malloc functions here. See mm-naive.c for an example implementation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* 
 * mm_init initializes the malloc package.
 */
int mm_init(void)
{
	// Your code here
	return 0;
}

/* 
 * mm_malloc allocates a memory block of size bytes
 */
void *mm_malloc(size_t size)
{
	// Your code here
	return NULL;
}

/*
 * mm_free frees the previously allocated memory block
 */
void mm_free(void *ptr)
{
	// Your code here
}	

/*
 * mm_realloc changes the size of the memory block pointed to by ptr to size bytes.  
 * The contents will be unchanged in the range from the start of the region up to the minimum of   
 * the  old  and  new sizes.  If the new size is larger than the old size, the added memory will   
 * not be initialized.  If ptr is NULL, then the call is equivalent  to  malloc(size).
 * if size is equal to zero, and ptr is not NULL, then the call is equivalent to free(ptr).
 */
void *mm_realloc(void *ptr, size_t size)
{
	// Your code here
	return NULL;
}


/*
 * mm_checkheap checks the integrity of the heap and helps with debugging
 */
void mm_checkheap(int verbose_level) 
{
	// Your code here
}

