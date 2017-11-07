# Makefile for IFJ project
# Author: Jiri Vozar

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
VPATH=src

all: ifj

ifj: main.o scanner.o strings.o errors.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

main.o: main.c scanner.h strings.h errors.h
parser.o: parser.c parser.h scanner.h strings.h errors.h
scanner.o: scanner.c scanner.h strings.h errors.h
strings.o: strings.c strings.h 

clean:
	rm -f ifj *.o

.PHONY: all clean
