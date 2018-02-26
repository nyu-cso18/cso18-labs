
/***********************************************************
 File Name: bloom.c
 Description: implementation of bloom filter goes here 
 **********************************************************/

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "rkgrep.h"

/* Constants for bloom filter implementation */
const int H1PRIME = 4189793;
const int H2PRIME = 3296731;

int BLOOM_HASH_NUM = 10;

/* The hash function used by the bloom filter */
int
hash_i(int i, /* which of the BLOOM_HASH_NUM hash functions to use */ 
       long long x /* the element (a RK value) to be hashed */)
{
	return ((x % H1PRIME) + i*(x % H2PRIME) + 1 + i*i);
}

/* Initialize a bloom filter by allocating a character array that can pack bsz bits.
 * The size of the character array is bsz/8 bytes
 * All bytes of the allocated character are set to zero (i.e. all bits in the bloom filter
 * are initialized to zero).
 */
bloom_filter *
bloom_init(int bsz /* size of bitmap in bits*/ )
{
	assert((bsz % 8) == 0);

	bloom_filter *bf = (bloom_filter *)malloc(sizeof(bloom_filter));
	bf->bsz = bsz;
	bf->buf = (char*)malloc(bsz>>3);
	if(!bf->buf) {
		printf("failed to alloc memory for bloom filter\n");
		exit(1);
	}
	memset(bf->buf, 0, bf->bsz >> 3);
	return bf;
}

/* Add elm into the given bloom filter. Obtain BLOOM_HASH_NUM bitmap positions by feeding 
 * elm to the given hash_i() function and set each position (modulo bitmap size) to 1.
 * We use a "big-endian" like bit-ordering convention for the bloom filter implemention.
 * As an example, to set bit-position-at-9 to 1, the function sets the left-most
 * bit of the second byte in the character array that represents the bitmap to 1.
*/
void
bloom_add(bloom_filter *bf,
	long long elm /* the element to be added (a RK hash value) */)
{
	/* Your code here */
}

/* Query if elm is in the given bloom filter (with high probability). Obtain 
 * BLOOM_HASH_NUM bitmap positions by feeding elm to the given hash_i() function 
 * and check whether those positions are set (i.e. have 1). If all those positions 
 * are set, then elm is found in the bloom filter and the function returns true. 
 * */ 
bool
bloom_query(bloom_filter *f,
	long long elm /* the query element (a RK hash value) */ )
{
	/* Your code here */
	return false;
}

void 
bloom_free(bloom_filter *bf)
{
	free(bf->buf);
	free(bf);
}


/* bloom_bit_at_pos returns whether the bit at position "pos" of the bitmap 
 * is set or not
 */
bool
bloom_bit_at_pos(bloom_filter *bf, int pos)
{
	assert(pos < bf->bsz);
	// which_byte maps "pos" position of the bitmap to f->buf array index
	int which_byte = pos / 8;
	// which_bit maps "pos" position of the bitmap to the bit position in f->buf[which_byte]
	int which_bit = pos % 8;
	// extract the bit at the which_bit position of the byte (f->buf[which_byte])
	return (bf->buf[which_byte] << which_bit) >> (7-which_bit);
}
