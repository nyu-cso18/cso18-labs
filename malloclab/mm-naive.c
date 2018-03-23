/*
 * mm-naive.c - An example malloc implementation that's fast but not memory-efficient.
 * 
 * In this naive approach, a chunk is allocated by incrementing
 * the brk pointer.  Chunks are "linked" together via implicit 
 * list with each chunk including a header (but not footer).
 * Blocks are never coalesced nor re-used.  Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

typedef struct {
	size_t size_n_status;
	size_t unused;  //pad 8 bytes so header is 16-byte aligned (then payload will always be 16-byte aligned)
} header;

void set_chunk_size_status(header *hdr, size_t size, bool status)
{
	hdr->size_n_status = size | (size_t)status;
}

void set_chunk_status(header *hdr, bool status)
{
	size_t mask = ~(1-(size_t)status);
	hdr->size_n_status &= mask;
}

bool get_chunk_status(header *hdr)
{
	return hdr->size_n_status & 0x1L;
}

size_t get_chunk_size(header *hdr)
{
	return hdr->size_n_status & (~0x1L);
}

header *payload_to_header(void *p)
{
	return (header *)((char *)p - sizeof(header));
}

void *header_to_payload(header *hdr)
{
	return (void *)((char *)hdr + sizeof(header));
}

header *get_next_chunk(header *hdr)
{
	size_t sz = get_chunk_size(hdr);
	return (header *)((char *)hdr + sz);
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	int newsize = align(size) + sizeof(header);
	void *h = (header *)mem_sbrk(newsize);
	if (h == (void *)-1)
		return NULL;
	else {
		assert(is_aligned((void *)h));
		set_chunk_size_status(h, newsize, true);
		assert(get_chunk_status(h) == true);
		assert(get_chunk_size(h) == newsize);
		return header_to_payload(h);
	}
	//mm_checkheap(0);
}

/*
 * mm_free - Freeing a block clears the status in implicit header
 */
void mm_free(void *ptr)
{
	assert(ptr >= mem_heap_lo() && ptr <= mem_heap_hi());
	header *h;
	h = payload_to_header(ptr);
	set_chunk_status(h, false);
	assert(get_chunk_status(h) == false);
	//mm_checkheap(0);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
	void *newptr = NULL;
	if (size > 0) {
		newptr = mm_malloc(size);
		if (newptr == NULL)
			return NULL;

		if (ptr != NULL) {
			header *h = payload_to_header(ptr); 
			size_t copySize = get_chunk_size(h) - sizeof(header); //payload size
			if (size < copySize) {
				//copySize is the minimum of new payload size and old payload size
				copySize = size;
			}
			memcpy(newptr, ptr, copySize); //copy the old payload into its new location
		}
	}
	if (ptr != NULL) {
	       	mm_free(ptr); 
	}
	return newptr;
}


void mm_checkheap(int verbose_level) 
{
	size_t total_allocated = 0, total_free = 0;
	size_t total_allocated_sz = 0, total_free_sz = 0;
	header *h;
	h = mem_heap_lo();
	// do a simple check by traversing all the chunks and print out their size and status
	while ((void *)h < mem_heap_hi()) {
		if (verbose_level > NORMAL_VERBOSE) {
		       	printf("chunk size %ld status %d\n", get_chunk_size(h), get_chunk_status(h));
		}
		if (get_chunk_status(h)) {
			total_allocated_sz += get_chunk_size(h);
			total_allocated++;
		} else {
			total_free_sz += get_chunk_size(h);
			total_free++;
		}
	       	h = get_next_chunk(h);
	}
	if (verbose_level > 0) {
	       	printf("total non-free chunks %ld size %ld, total free chunks %ld size %ld\n", total_allocated, total_allocated_sz, total_free, total_free_sz);
	}
	return;
}

