CC = gcc
CFLAGS = -Wall  -Wno-unused-result -std=gnu99 -Og -g

OBJS = mdriver.o memlib.o fsecs.o fcyc.o clock.o ftimer.o

all: mdriver mdriver-naive

mdriver: $(OBJS) mm.o
	$(CC) $(CFLAGS) -o $@ $^

mdriver-naive: $(OBJS) mm-naive.o
	$(CC) $(CFLAGS) -o $@ $^

mdriver.o: mdriver.c fsecs.h fcyc.h clock.h memlib.h config.h mm.h
memlib.o: memlib.c memlib.h
mm.o: mm.c mm.h memlib.h
fsecs.o: fsecs.c fsecs.h config.h
fcyc.o: fcyc.c fcyc.h
ftimer.o: ftimer.c ftimer.h config.h
clock.o: clock.c clock.h

clean:
	rm -f *~ *.o mdriver  mdriver-naive


