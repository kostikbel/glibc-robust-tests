CC=gcc
CXX=g++
CFLAGS_COMMON=-O -g -Wall -Wextra
CFLAGS=-std=c11 $(CFLAGS_COMMON)
CXXFLAGS=$(CFLAGS_COMMON)

BINS=robust rh-pr628608 tst-robust1 tst-robust2 tst-robust3
all:	$(BINS)

robust:	robust.c
	$(CC) $(CFLAGS) -o robust robust.c -lpthread

rh-pr628608:	rh-pr628608.cc
	$(CXX) $(CXXFLAGS) -o rh-pr628608 rh-pr628608.cc -lpthread

tst-robust1:	tst-robust1.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robust1 tst-robust1.c -lpthread

tst-robust2:	tst-robust2.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robust2 tst-robust2.c -lpthread

tst-robust3:	tst-robust3.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robust3 tst-robust3.c -lpthread

clean:
	rm -f $(BINS) *.core
