CC=gcc
CXX=g++
CFLAGS_COMMON=-O -g -Wall -Wextra
CFLAGS=-std=c11 $(CFLAGS_COMMON)
CXXFLAGS=$(CFLAGS_COMMON)

BINS= \
	robust \
	rh-pr628608 \
	tst-robust1 \
	tst-robust2 \
	tst-robust3 \
	tst-robust4 \
	tst-robust5 \
	tst-robust6 \
	tst-robust7 \
	tst-robustpi1 \
	tst-robustpi2 \
	tst-robustpi3 \
	tst-robustpi4 \
	tst-robustpi5 \
	tst-robustpi6 \
	tst-robustpi7

all:	$(BINS)

do_tests:
	@for x in 1 2 3 4 5 6 7; do \
	    echo tst-robust$$x ;\
	    ./tst-robust$$x ;\
	done
	@for x in 1 2 3 4 5 6 7; do \
	    echo tst-robustpi$$x ;\
	    ./tst-robustpi$$x ;\
	done

robust:	robust.c
	$(CC) $(CFLAGS) -o robust robust.c -lpthread

rh-pr628608:	rh-pr628608.cc
	$(CXX) $(CXXFLAGS) -o rh-pr628608 rh-pr628608.cc -lpthread

tst-robust1:	tst-robust1.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robust1 tst-robust1.c -lpthread

tst-robust2:	tst-robust2.c test-skeleton.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robust2 tst-robust2.c -lpthread

tst-robust3:	tst-robust3.c test-skeleton.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robust3 tst-robust3.c -lpthread

tst-robust4:	tst-robust4.c test-skeleton.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robust4 tst-robust4.c -lpthread

tst-robust5:	tst-robust5.c test-skeleton.c tst-robust2.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robust5 tst-robust5.c -lpthread

tst-robust6:	tst-robust6.c test-skeleton.c tst-robust3.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robust6 tst-robust6.c -lpthread

tst-robust7:	tst-robust7.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robust7 tst-robust7.c -lpthread

tst-robustpi1:	tst-robustpi1.c test-skeleton.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robustpi1 tst-robustpi1.c -lpthread

tst-robustpi2:	tst-robustpi2.c test-skeleton.c tst-robustpi1.c \
    tst-robust2.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robustpi2 tst-robustpi2.c -lpthread

tst-robustpi3:	tst-robustpi3.c test-skeleton.c tst-robust3.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robustpi3 tst-robustpi3.c -lpthread

tst-robustpi4:	tst-robustpi4.c test-skeleton.c tst-robust4.c tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robustpi4 tst-robustpi4.c -lpthread

tst-robustpi5:	tst-robustpi5.c test-skeleton.c tst-robust5.c tst-robust2.c \
    tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robustpi5 tst-robustpi5.c -lpthread

tst-robustpi6:	tst-robustpi6.c test-skeleton.c tst-robust6.c tst-robust3.c \
    tst-robust1.c
	$(CC) $(CFLAGS) -o tst-robustpi6 tst-robustpi6.c -lpthread

tst-robustpi7:	tst-robustpi7.c tst-robust7.c test-skeleton.c
	$(CC) $(CFLAGS) -o tst-robustpi7 tst-robustpi7.c -lpthread

clean:
	rm -f $(BINS) *.core
