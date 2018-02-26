CFLAGS=-g -O3 -std=gnu99

all: rkgrep rkgrep_test

rkgrep: rkgrep.o bloom.o rkgrep_main.o
	gcc $^ -o $@ -lrt -lm

rkgrep_test: rkgrep_test.o rkgrep.o bloom.o rkgrep_harness.o
	gcc $^ -o $@ -lrt -lm 

%.o : %.c
	gcc $(CFLAGS) -DANSWER=$(ANSWER) -c ${<}

clean :
	rm -f rkgrep.o rkgrep_main.o bloom.o rkgrep_test.o rkgrep rkgrep_test 
