#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>

#include "panic_cond.h"
#include "part7.h"

extern tnode_t *init_tree();
extern int insert(tnode_t *tree, char *s);
extern int preorder(tnode_t *tree, char **result, int max_result);
extern int inorder(tnode_t *tree, char **result, int max_result);
extern void del_tree(tnode_t *tree);

static char *
concat_strings(char **list, int n)
{
	int debug_strlen = 10000;
	char *debug_str = (char *)malloc(debug_strlen);
	assert(debug_str);
	int len = 0;
	for (int i = 0; i < n; i++) {
		len += sprintf(debug_str+len, "%s ", list[i]);
	       	assert(len < debug_strlen);
	}
	return debug_str;
}

static int strcmp_for_qsort(const void * a, const void * b)
{
	return strcmp(*(const char **) a, *(const char **) b);
}

// create num_strings strings with each string having n_chars characters
char **
gen_rand_strings(int num_strings, int n_chars)
{
	// strings is an array of char* 
	char **strings = (char **)malloc(sizeof(char *)*num_strings);
	for (int i = 0; i < num_strings; i++) {
		strings[i] = (char *)malloc(n_chars+1);
		for (int j = 0; j < n_chars; j++) {
			strings[i][j] = rand() % 9 + '1';
		}
		strings[i][n_chars] = '\0';
	}
	return strings;
}

void
free_strings(char **strings, int num_strings)
{
	for (int i = 0; i < num_strings; i++) {
		free(strings[i]);
	}
	free(strings);
}

void
challenge_question(int num_nodes) 
{
	char **strings = gen_rand_strings(num_nodes, 20);
	printf("About to insert %d random strings in random order\n", num_nodes);
	tnode_t *tree = init_tree();
	for (int i = 0; i < num_nodes; i++) {
		insert(tree, strings[i]);
	}
	del_tree(tree);
	printf("Successfully inserted %d random strings in random order\n", num_nodes);

	qsort(strings, num_nodes, sizeof(char *), strcmp_for_qsort);
	printf("About to insert %d random strings in sorted order\n", num_nodes);
	tree = init_tree();
	for (int i = 0; i < num_nodes; i++) {
		insert(tree, strings[i]);
	}
	del_tree(tree);
	printf("Successfully inserted %d random strings in sorted order\n", num_nodes);
	free_strings(strings, num_nodes);
}

int
main(int argc, char **argv)
{

	int getopt(int argc, char * const argv[],
			                  const char *optstring);
	if (argc > 1) {
		// When commandline option is specified, we
		// run the following test for the challenge question 
		printf("Challenge question test\n");
		int num_nodes = 0;
		char c;
		while ((c = getopt(argc, argv, "n:")) != -1) {
			if (c == 'n') {
				num_nodes = atoi(optarg);
				break;
			}
		}
		if (num_nodes <= 0) {
			printf("please specify a positive node number instead of %d\n", num_nodes);
			exit(1);
		}
		challenge_question(num_nodes);
		return 0;
				 
	}
#define NUM_TESTS 5
#define NUM_NODES 5
	char *test[NUM_TESTS][NUM_NODES] = {{"000", "111", "222", "333", "444"},
			    {"444", "333", "222", "111", "000"},
			    {"222", "111", "333", "000", "444"},
			    {"000", "111", "333", "222", "444"},
			    {"222", "000", "444", "111", "333"}};

	char *preorder_results[NUM_TESTS][NUM_NODES] = {{"000", "111", "222", "333", "444"},
	       		    {"444", "333", "222", "111", "000"},
			    {"222", "111", "000", "333", "444"},
			    {"000", "111", "333", "222", "444"},
			    {"222", "000", "111", "444", "333"}};

	tnode_t *tree;

	char **sorted= (char **)malloc(sizeof(char *)*NUM_NODES);
	for (int j = 0; j < NUM_NODES; j++) {
		sorted[j] = test[0][j];
	}
	qsort(sorted, NUM_NODES, sizeof(char *), strcmp_for_qsort);


	char **result = (char **)malloc(sizeof(char *)*NUM_NODES);

	for (int i = 0; i < NUM_TESTS; i++) {
		printf("   >>> part7: testing %d-th tree: insert %d strings, traverse preorder, delete tree\n", i, NUM_NODES);
	       	tree = init_tree();
	       	for (int j = 0; j < NUM_NODES; j++) {
			int ret = insert(tree, test[i][j]);
		       	panic_cond(ret == 0, "insert %d-th string %s failed", j, test[i][j]);
			printf("   part7: inserted string %s\n", test[i][j]);
		}
		int ret = preorder(tree, result, NUM_NODES);
		panic_cond(ret == NUM_NODES, "Returned %d nodes, expected %d", ret, NUM_NODES);
		for (int j = 0; j < NUM_NODES; j++) {
			panic_cond(strcmp(preorder_results[i][j], result[j])== 0, 
					"preorder traversal: expected (%s), got (%s)\n", concat_strings(preorder_results[i], NUM_NODES),
					concat_strings(result, NUM_NODES));
		}			
		del_tree(tree);
	}
	printf("part7: preorder_traversal OK\n");

	for (int i = 0; i < NUM_TESTS; i++) {
		printf("   >>> part7: testing %d-th tree: insert %d strings, traverse inorder, delete tree\n", i, NUM_NODES);
	       	tree = init_tree();
	       	for (int j = 0; j < NUM_NODES; j++) {
			int ret = insert(tree, test[i][j]);
		       	panic_cond(ret == 0, "insert %d-th string %s failed", j, test[i][j]);
			printf("   part7: inserted string %s\n", test[i][j]);
		}

		int ret = inorder(tree, result, NUM_NODES);
		panic_cond(ret == NUM_NODES, "Returned %d nodes, expected %d", ret, NUM_NODES);
		for (int j = 0; j < NUM_NODES; j++) {
			panic_cond(strcmp(sorted[j], result[j])== 0, 
					"inorder traversal: expected (%s), got (%s)\n", concat_strings(sorted, NUM_NODES),
					concat_strings(result, NUM_NODES));
		}			
		del_tree(tree);
	}

	printf("part7: inorder_traversal OK\n");
	free(result);

	// do a large inorder test with duplicates 
	int num_nodes = 1000; //there are <1000 possible values, so some must be duplicates among 1000
	char **strings = gen_rand_strings(num_nodes, 3);
	bool *seen_before = (bool *)malloc(sizeof(bool)*num_nodes);
	bzero((void *)seen_before, sizeof(bool)*num_nodes);
	tree = init_tree();
	for (int i = 0; i < num_nodes; i++) {
		int val = atoi(strings[i]);
		assert(val > 0 && val < num_nodes);
		int ret = insert(tree, strings[i]);
		if (!seen_before[val]) {
			panic_cond(ret == 0, "insert %d-th string (%s) failed\n", i, strings[i]);
			seen_before[val] = strings[i];
		} else {
			panic_cond(ret == 1, "insert %d-th string (%s) should have returned 1 for duplicates\n", i, strings[i]);
		}
	}

	qsort(strings, num_nodes, sizeof(char *), strcmp_for_qsort);
	char **nodup= (char **)malloc(sizeof(char *)*num_nodes);
	int nodup_i = 0;
	for (int i = 0; i < num_nodes; i++) {
		if (nodup_i == 0 || strcmp(strings[i],nodup[nodup_i-1])!=0) {
			nodup[nodup_i] = strings[i];
			nodup_i++;
		}
	}

	// nodup (of size nodup_i) contains strings without duplication and 
	// there should be nodup_i nodes in the tree
	result = (char **)malloc(sizeof(char *)*num_nodes);
	int ret = inorder(tree, result, num_nodes);
	panic_cond(ret == nodup_i, "Returned %d nodes, expected %d", ret, nodup_i);
	for (int i = 0; i < nodup_i; i++) { 
		panic_cond(strcmp(result[i], nodup[i]) == 0, "inorder traversal ordering is wrong %d-th node should be %s not %s", i, nodup[i], result[i]);
	}
	del_tree(tree);
	free(result);
	free(nodup);
	free(seen_before);
	free_strings(strings, num_nodes);
	printf("part7: inorder_traversal with duplicates OK\n");
	return 0;
}
