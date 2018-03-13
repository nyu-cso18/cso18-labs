/*
 * Unit tests for Rabin Karp lab.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "rkgrep.h"
#include "panic_cond.h"

#define NUM_TESTS 5

int test_pattern_len = 800;
int test_document_len = 100000;
int test_bloom_bsz_large = 8000;
int test_bloom_bsz_small  = 80;

/* reference implementation in rkgrep_harness used for testing */
long long rkhash_init_1(const char *charbuf, int k, long long *h);
long long rkhash_next_1(long long curr_hash, long long h, char prev, char next);
bloom_filter *bloom_init_1(int bsz);
void bloom_add_1(bloom_filter *f, long long elm);
bloom_filter *rk_create_doc_bloom_1(int m, const char *doc, int bloom_size);

long long
timediff(struct timespec ts, struct timespec ts0)
{
	return (ts.tv_sec-ts0.tv_sec)*1000000+(ts.tv_nsec-ts0.tv_nsec)/1000;
}

void
generate_random_word(char *buf, int len)
{
	for (int i = 0; i < len; i++) {
		buf[i] = 'a' + rand() % 26;
	}
	buf[len] = '\0';
}

// generate a random normalized string of containing a sequence of words. The string is null-terminated
// and has at most len bytes.
char *
generate_random_document(int len)
{
	char* buf = (char *)malloc(len+1);
	int i = 0;
	while (i < len) {
		int word_len = rand() % 5 + 5;
		if (word_len > (len-i)) {
			word_len = len -i;
		} else if (word_len == (len-i-1)) {
			word_len++;
		}
		generate_random_word(buf+i, word_len);
		i += word_len;
		if (i == len) {
			break;
		}
		buf[i] = ' ';
	}
	buf[len] = '\0';
	return buf;
}

long long 
test_match_at_pos(int (*matchFunc)(const char *, const char *, int *), char *doc, int pos, int expected_n_matches, int pattern_len, bloom_filter *bf)
{
	char *p = (char *) calloc(pattern_len+1, sizeof(char));
	if (pos >= 0) {
	       	strncpy(p, doc + pos, pattern_len);
	} else {
		generate_random_word(p, pattern_len);
	       	p[pattern_len-1] = '!'; // make sure this pattern never matches
	}

	struct timespec ts1, ts2;
	clock_gettime(CLOCK_REALTIME, &ts1);
	int n_matches;
	int first_match_ind;
	if (bf) {
		n_matches = rk_substring_match_using_bloom(p, doc, bf, &first_match_ind);
	} else {
	       	n_matches = (*matchFunc)(p, doc, &first_match_ind);
	}
	clock_gettime(CLOCK_REALTIME, &ts2);
	if (expected_n_matches >= 0) {
	       	panic_cond(n_matches == expected_n_matches, "Number of matched positions is %d (should be %d)", n_matches, expected_n_matches);
	}

	if (expected_n_matches != 0) {
	       	panic_cond(first_match_ind == pos, "Pattern (%s) found in doc (%s) at position %d != %d (expected)\n", p, doc, first_match_ind, pos);
	}
       	free(p);
	return timediff(ts2, ts1);
}

void
test_naive()
{
	printf("== test_naive==\n");
	// test short pattern matching in a short doc
	char* doc = "abracadabra";
	int short_pattern_len = 3;
	// there are 2 matches for "abr" in "abracadabra"
	test_match_at_pos(&naive_substring_match, doc, 0, 2, short_pattern_len, NULL);
	// there is 1 match for "dab" in "abracadabra"
	test_match_at_pos(&naive_substring_match, doc, 6, 1, short_pattern_len, NULL);

	// test non-matches
	char *pattern = "aaa";
	int pos;
	int n_matches = naive_substring_match(pattern, doc, &pos);
	panic_cond(n_matches == 0, "Pattern (%s) should not have been found in doc (%s)\n", pattern, doc);
	printf("finished testing small pattern matching in doc (%s)\n", doc);

	// test worse case performance
	// pattern = "aa...b"
	// document = "aaaaaaaa....[pattern]...aaaaa"
	int count = 0;
	long long duration_sum = 0;
	doc = malloc(test_document_len+1);
	memset(doc, 'a', test_document_len);
	doc[test_document_len] = '\0';
	while (count < 100) {
		pos = rand() % (test_document_len - test_pattern_len);
		doc[pos+test_pattern_len-1] = 'b';
		duration_sum += test_match_at_pos(&naive_substring_match, doc, pos, 1, test_pattern_len, NULL);
		doc[pos+test_pattern_len-1] = 'a';
		count++;
	}
	printf("avg worse-case runtime %lld (microseconds) out of %d matches\n", duration_sum/count, count);
	free(doc);

	printf("-- test_naive: OK --\n");
}

void
test_rkhash(int pattern_len, char *doc)
{
	long long h;
	long long hash = rkhash_init(doc, pattern_len, &h);

	long long h1;
	long long hash1 = rkhash_init_1(doc, pattern_len, &h);
       	panic_cond(hash == hash1, "rkhash_init returns %lld != %lld (expected)\n", hash, hash1);

	int dlen = strlen(doc);
	for (int i = pattern_len; i < dlen; i++) {
		hash = rkhash_next(hash, h, doc[i-pattern_len], doc[i]);
		hash1 = rkhash_next_1(hash1, h, doc[i-pattern_len], doc[i]);
		panic_cond(hash == hash1, "test_rkhash_next i=%d rkhash_next returns %lld != %lld (expected)\n", i, hash, hash1);
	}
}

void
test_rk()
{
	printf("== test_rk==\n");

	char *doc = "abracadabra";
	int short_pattern_len = 3;
	test_rkhash(short_pattern_len, doc);

	// test non-match
	char *pattern = "aaa";
	int pos;
	int n_matches = rk_substring_match(pattern, doc, &pos);
	panic_cond(n_matches == 0, "Pattern (%s) should not have been found in doc (%s)\n", pattern, doc);

	printf("finished testing small pattern matching in doc (%s)\n", doc);

	// test worse case performance
	int count = 0;
	long long duration_sum = 0;
	doc = malloc(test_document_len+1);
	memset(doc, 'a', test_document_len);
	doc[test_document_len] = '\0';
	while (count < 100) {
		pos = rand() % (test_document_len - test_pattern_len);
		doc[pos+test_pattern_len-1] = 'b';
		duration_sum += test_match_at_pos(&rk_substring_match, doc, pos, 1, test_pattern_len, NULL);
		doc[pos+test_pattern_len-1] = 'a';
		count++;
	}
	printf("avg worse-case runtime %lld (microseconds) out of %d matches\n", duration_sum/count, count);
	free(doc);

	printf("-- test_rk: OK --\n");

}

void
test_bloom()
{
	printf("== test_bloom ===\n");
	bloom_filter *bf = bloom_init(test_bloom_bsz_small);
	bloom_filter *bf1 = bloom_init_1(test_bloom_bsz_small);
	int x = rand() % 100;
	bloom_add(bf, x);
	bloom_add_1(bf1, x);
	printf("inserted %d into %d-bit bloom filter\n", x, test_bloom_bsz_small);
	for (int i = 0; i < test_bloom_bsz_small; i++) {
		bool b = bloom_bit_at_pos(bf, i);
	        bool b1 = bloom_bit_at_pos(bf1, i); 
		panic_cond(b == b1, "Bloom filter pos %d bit value is %d != %d (expected)\n", i, b, b1);
	}
	panic_cond(bloom_query(bf, x) == true, "Bloom filter should contain %lld\n", x);
	printf("%d-bit bloom filter after insertions matches expected\n", test_bloom_bsz_small);

	bloom_free(bf);
	bloom_free(bf1);

	bf = bloom_init(test_bloom_bsz_large);
	bf1 = bloom_init_1(test_bloom_bsz_large);
	int n_inserted = test_bloom_bsz_large / 10;
	long long *test_numbers = (long long *)malloc(sizeof(long long)*n_inserted);
	for (int i = 0; i < n_inserted; i++) {
	       	long long x = rand();
	       	x = x<< 31 | rand();
		test_numbers[i] = x;
		bloom_add(bf, x);
	       	bloom_add_1(bf1, x);
	}
	printf("inserted %d numbers into %d-bit bloom filter\n", n_inserted, test_bloom_bsz_large);

	for (int i = 0; i < n_inserted; i++) {
		panic_cond(bloom_query(bf, test_numbers[i]), "Bloom filter should contain %lld\n", test_numbers[i]);
	}

	for (int i = 0; i < test_bloom_bsz_large; i++) {
		bool b = bloom_bit_at_pos(bf, i);
	        bool b1 = bloom_bit_at_pos(bf1, i); 
		panic_cond(b == b1, "Bloom filter pos %d bit value is %d != %d (expected)\n", i, b, b1);
	}
	printf("%d-bit bloom filter after insertions matches expected\n", test_bloom_bsz_large);

	bloom_free(bf);
	bloom_free(bf1);
	printf("-- test_bloom: OK --\n");
}

void
test_rk_bloom()
{
	printf("== test_rk_bloom ===\n");
	char *doc = "123 456 789";
	char *pattern = "456";
	bloom_filter *bf = rk_create_doc_bloom(strlen(pattern), doc, test_bloom_bsz_small);
	bloom_filter *bf1 = rk_create_doc_bloom_1(strlen(pattern), doc, test_bloom_bsz_small);
	for (int i = 0; i < test_bloom_bsz_small; i++) {
		bool b = bloom_bit_at_pos(bf, i);
		bool b1 = bloom_bit_at_pos(bf1, i);
		panic_cond(b == b1, "Bloom filter pos %d bit value is %d != %d (expected)\n", i, b, b1);
	}
 
	int pos;
	int n_matches = rk_substring_match_using_bloom(pattern, doc, bf, &pos);
       	panic_cond(pos == 4, "rk_substring_match_using_bloom returned %d != %d (expected)\n", pos, strstr(doc, pattern)-doc);
	printf("finished testing small pattern matching in doc (%s)\n", doc);

	doc = generate_random_document(test_document_len);
	int bloom_size = test_document_len*10;
	bf = rk_create_doc_bloom(test_pattern_len, doc, bloom_size);
	bf1 = rk_create_doc_bloom_1(test_pattern_len, doc, bloom_size);
	for (int i = 0; i < bloom_size; i++) {
		bool b = bloom_bit_at_pos(bf, i);
		bool b1 = bloom_bit_at_pos(bf1, i);
		panic_cond(b == b1, "Bloom filter pos %d bit value is %d != %d (expected)\n", b, b1);
	}

	long long duration_sum = 0;
	int count = 0;
	while (count < 100) {
		if ((rand() % 10) >= 1) {
			pos = -1;
			n_matches = 0;
		} else {
			pos = rand() % (test_document_len - test_pattern_len);
			n_matches = 1;
		}
		duration_sum += test_match_at_pos(NULL, doc, pos, n_matches, test_pattern_len, bf);
		count++;
	}
	printf("avg good case runtime %lld (microseconds)\n", duration_sum/count);
	printf("-- test_rk_bloom: OK --\n");
}

int
main(int argc, char **argv)
{

	enum algo_type which_test = All;
	char c;
        unsigned int seed = 42;
	while ((c = getopt(argc, argv, "a:m:n:s:r")) != -1) {
		switch (c) {
			case 'a':
				if (strcmp(optarg, "naive") == 0) {
					which_test = Naive;
				} else if (strcmp(optarg, "rk") == 0) {
					which_test = RK;
				} else if (strcmp(optarg, "bloom") == 0) {
					which_test = Bloom;
				} else if (strcmp(optarg, "rkbloom") == 0) {
					which_test = RKBloom;
				} else {
					printf("unknown test type %s", optarg);
				       	exit(1);
				}
				break;
			case 'm':
				test_pattern_len = atoi(optarg);
				break;
			case 'n':
				test_document_len = atoi(optarg);
				break;
                        case 's':
                                seed = (unsigned) atoi(optarg);
                                break;
                        case 'r':
                                seed = time(NULL);
                                break;
			default:
				printf("rkgrep -a <test type>\n");
				exit(1);
		}
	}
        printf("Using seed %u\n", seed);
        srand(seed);

	printf("test_pattern_len=%d test_document_len=%d\n", test_pattern_len, test_document_len);
	if (which_test == Naive || which_test == All) {
	       	test_naive();
	}

	if (which_test == RK || which_test == All) {
	       	test_rk();
	}

	if (which_test == Bloom || which_test == All) {
	       	test_bloom();
	}

	if (which_test == RKBloom || which_test == All) {
	       	test_rk_bloom();
	}
}
