/* Match every k-character snippet of the query_doc document
	 among a collection of documents doc1, doc2, ....

	 ./rkmatch snippet_size query_doc doc1 [doc2...]

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "bloom.h"
#include "rkgrep.h"

#define MAX_PATTERNS 1000

#define NORMALCOLOR "\x1B[0m"
#define REDCOLOR "\x1B[31m"

/* read the entire content of the file 'fname' into a 
	 character array allocated by this procedure.
	 Upon return, *doc contains the address of the character array
	 *doc_len contains the length of the array
	 */
char *
read_ascii_file(const char *fname) 
{

	int fd = open(fname, O_RDONLY);
	if (fd < 0) {
		perror("read_ascii_file: open ");
		exit(1);
	}

	struct stat st;
	if (fstat(fd, &st) != 0) {
		perror("read_ascii_file: fstat ");
		exit(1);
	}

	char *doc = (char *)malloc(st.st_size + 1);
	if (!doc) {
		fprintf(stderr, " failed to allocate %d bytes. No memory\n", (int)st.st_size);
		exit(1);
	}

	int n = read(fd, doc, st.st_size);
	if (n < 0) {
		perror("read_ascii_file: read ");
		free(doc);
		doc = NULL;
	}else if (n != st.st_size) {
		fprintf(stderr,"read_ascii_file: short read!\n");
		free(doc);
		doc = NULL;
	} else {
		// check if all characters read are ASCII
		for (int i = 0; i < st.st_size; i++) {
			if (doc[i] <= 0) {
				// not a valid ascii character
				fprintf(stderr, "character (%c) is not ASCII\n", doc[i]);
				free(doc);
				doc = NULL;
			}
		}
	}
	close(fd);
	if (!doc) {
		doc[st.st_size] = '\0';
	}
	return doc;
}

void
print_matched_sentence(int pos, char *pattern, char *doc) 
{
	if (pos < 0) {
		return;
	}
	// find and print the beginning of the sentence that contains the matched pattern
	int start_sentence = pos;
	while (start_sentence > 0 && doc[start_sentence-1] != '\n') {
		start_sentence--;
	}
	for (int j = start_sentence; j < pos; j++) {
		printf("%c", doc[j]);
	}
	// print the matched pattern in red
	printf("%s", REDCOLOR);
	int i = 0;
	while (doc[i+pos] == pattern[i]) {
		printf("%c", doc[i+pos]);
		i++;
	}
	// print the rest of the sentence in normal color
	printf("%s", NORMALCOLOR);
	while (doc[i+pos] != '\n' && doc[i+pos] != '\0') {
		printf("%c", doc[i+pos]);
		i++;
	}
	printf("\n");
}

int 
main(int argc, char **argv)
{
	enum algo_type which_algo = RK; /* default match algorithm is simple */
	
	/* Refuse to run on platform with a different size for long long*/
	assert(sizeof(long long) == 8);

	/*getopt is a C library function to parse command line options */
	int c;
	while ((c = getopt(argc, argv, "a:")) != -1) {
	       	switch (c) {
			case 'a':
				if (strcmp(optarg, "naive") == 0) {
					which_algo = Naive;
				} else if (strcmp(optarg, "rk") == 0) {
					which_algo = RK;
				} else if (strcmp(optarg, "rkbloom") == 0) {
					which_algo = RKBloom;
				} else {
					printf("unknown test type %s", optarg);
				       	exit(1);
				}
				break;
			default:
				printf("rkgrep -a <test type> pattern1|pattern2|pattern3 <filename>\n");
				exit(1);
		}
       	}

	/* optind is a global variable set by getopt() 
		 it now contains the index of the first argv-element 
		 that is not an option*/
	if (argc - optind < 1) {
		printf("rkgrep -a <test type> pattern1|pattern2|pattern3 <filename>\n");
		exit(1);
	}

	char *patterns[MAX_PATTERNS];
	char *ind = NULL;
	char *ptr = strtok_r(argv[optind], "|", &ind);
	int n_patterns = 0;
	while (ptr != NULL) {
		patterns[n_patterns++] = ptr;
		assert(n_patterns < MAX_PATTERNS);
	       	ptr = strtok_r(NULL, "|", &ind);
	}
	if (!n_patterns) {
		printf("rkgrep -a <test type> pattern1|pattern2|pattern3 <filename>\n");
		exit(1);
	}

	char* doc = read_ascii_file(argv[optind+1]);
	if (!doc) {
		exit(1);
	}
	
	switch (which_algo) {
            case Naive:
		    for (int i = 0; i < n_patterns; i++) {
			    int first_match_ind;
			    int n_matches = naive_substring_match(patterns[i], doc, &first_match_ind);
			    print_matched_sentence(first_match_ind, patterns[i], doc);
			    if (n_matches > 1) {
				    printf("--  only 1 out %d matches for pattern %s is displayed\n", n_matches, patterns[i]);
			    }
		    }
		    break;
	    case RK:
		    for (int i = 0; i < n_patterns; i++) {
			    int first_match_ind;
			    int n_matches = rk_substring_match(patterns[i], doc, &first_match_ind);
			    print_matched_sentence(first_match_ind, patterns[i], doc);
			    if (n_matches > 1) {
				    printf("--  only 1 out %d matches for pattern %s is displayed\n", n_matches, patterns[i]);
			    }
		    }
	       	break;
	    case RKBloom: 
		{
			int m = strlen(patterns[0]);
			for (int i = 1; i < n_patterns; i++) {
				assert(m == strlen(patterns[i]));
			}
			bloom_filter *bf = rk_create_doc_bloom(m, doc, strlen(doc)*8);
			for (int i = 0; i < n_patterns; i++) {
				int first_match_ind;
				int n_matches = rk_substring_match_using_bloom(patterns[i], doc, bf, &first_match_ind);
				print_matched_sentence(first_match_ind, patterns[i], doc);
				if (n_matches > 1) {
				       	printf("--  only 1 out %d matches for pattern %s is displayed\n", n_matches, patterns[i]);
			       	}
			}
		}
		break;
	    default :
		printf("Unknown algo type %d\n", which_algo);
	       	exit(1);
	}
	free(doc);
	return 0;
}
