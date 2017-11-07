# Makefile for IFJ project
# Author: Jiri Vozar

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
VPATH=src:tests

all: ifj

ifj: main.o scanner.o strings.o errors.o
	$(CC) $(CFLAGS) $^ -o $@

sctest: scanner_tests.o scanner.o strings.o errors.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

errors.o: errors.c errors.h
main.o: main.c scanner.h strings.h errors.h
parser.o: parser.c parser.h scanner.h strings.h
scanner.o: scanner.c scanner.h strings.h errors.h
strings.o: strings.c strings.h

scanner_tests.o: scanner_tests.c strings.h scanner.h strings.h

clean:
	rm -f ifj sctest *.o

.PHONY: all clean
