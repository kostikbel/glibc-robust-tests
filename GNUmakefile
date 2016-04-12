CC=gcc
CXX=g++
CFLAGS=-O -g -Wall -Wextra

all:	robust rh-pr628608

robust:	robust.c
	$(CC) $(CFLAGS) -o robust robust.c -lpthread

rh-pr628608:	rh-pr628608.cc
	$(CXX) $(CFLAGS) -o rh-pr628608 rh-pr628608.cc -lpthread
