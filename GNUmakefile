CC=gcc
CXX=g++
CFLAGS=-O -g -Wall -Wextra

BINS=robust rh-pr628608 tst-robust1
all:	$(BINS)

robust:	robust.c
	$(CC) $(CFLAGS) -o robust robust.c -lpthread

rh-pr628608:	rh-pr628608.cc
	$(CXX) $(CFLAGS) -o rh-pr628608 rh-pr628608.cc -lpthread

tst-robust1:	tst-robust1.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robust1 tst-robust1.c -lpthread

clean:
	rm -f $(BINS) *.core
