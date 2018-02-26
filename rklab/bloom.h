#ifndef __BLOOM_H_
#define __BLOOM_H_

#include <stdbool.h>

typedef struct {
	char *buf; /* the bitmap representing the bloom filter*/
	int bsz; /* size of bitmap in bits*/
} bloom_filter;

bloom_filter *bloom_init(int bsz);
void bloom_free(bloom_filter *f);
int hash_i(int i, long long x);

void bloom_add(bloom_filter *f, long long elm);
bool bloom_query(bloom_filter *f, long long elm);

bool bloom_bit_at_pos(bloom_filter *f, int pos);

extern int BLOOM_HASH_NUM;

#endif

