# Makefile for IFJ project
# Author: Jiri Vozar

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g
VPATH=src:tests

all: ifj

ifj: main.o errors.o expressions.o functions.o parser.o scanner.o \
 stack.o statements.o strings.o symtable.o
	$(CC) $(CFLAGS) $^ -o $@

sctest: scanner_tests.o errors.o scanner.o strings.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

errors.o: errors.c errors.h scanner.h strings.h
expressions.o: expressions.c errors.h expressions.h scanner.h strings.h \
 functions.h symtable.h parser.h statements.h
functions.o: functions.c errors.h expressions.h scanner.h strings.h \
 functions.h symtable.h parser.h statements.h
main.o: main.c errors.h parser.h scanner.h strings.h symtable.h
parser.o: parser.c errors.h functions.h symtable.h scanner.h strings.h \
 parser.h statements.h
scanner.o: scanner.c scanner.h strings.h errors.h
stack.o: stack.c stack.h scanner.h strings.h errors.h
statements.o: statements.c errors.h expressions.h scanner.h strings.h \
 functions.h symtable.h parser.h statements.h
strings.o: strings.c strings.h
symtable.o: symtable.c symtable.h scanner.h strings.h errors.h

scanner_tests.o: scanner_tests.c strings.h scanner.h strings.h

tags: src/*
	ctags -R .

clean:
	rm -f ifj sctest *.o

.PHONY: all clean
