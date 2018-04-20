all := tester
OBJS:= htable.o rwlock.o testhash.o testrwlock.o tester.o

CC     := gcc
CFLAGS := -g -std=gnu99 -DANSWER=0

%.o : %.c
	$(CC) -c $(CFLAGS) $*.c


tester: $(OBJS)
	$(CC) -o $@ $^ -lpthread -lm 

clean : 
	rm -f tester $(OBJS) 
