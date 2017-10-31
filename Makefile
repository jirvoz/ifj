# Makefile for IFJ project
# Author: Jiri Vozar

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
VPATH=src

all: ifj

ifj: main.o scanner.o strings.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

main.o: main.c scanner.h strings.h
parser.o: parser.c parser.h scanner.h strings.h
scanner.o: scanner.c scanner.h strings.h
strings.o: strings.c strings.h

clean:
	rm -f ifj *.o

.PHONY: all clean
