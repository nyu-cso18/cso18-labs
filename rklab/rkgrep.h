#ifndef __RKGREP_H_
#define __RKGREP_H_

#include "bloom.h"

enum algo_type {Naive, RK, Bloom, RKBloom, All};

long long madd(long long a, long long b);
long long msub(long long a, long long b);
long long mmul(long long a, long long b);
long long rkhash_init(const char *charbuf, int k, long long *h);
long long rkhash_next(long long curr_hash, long long h, char prev, char next);

int naive_substring_match(const char *pattern, const char *doc, int *first_match_ind);
int rk_substring_match(const char *pattern, const char *doc, int *first_match_ind);
bloom_filter *rk_create_doc_bloom(int m, const char *doc, int bloom_size);
int rk_substring_match_using_bloom(const char *pattern, const char *doc, bloom_filter *bf, int *first_match_ind);

#endif
