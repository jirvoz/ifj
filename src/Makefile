# Makefile for compiling from src directory
# Author: Jiri Vozar

CC=gcc
CFLAGS=-std=c99 -O2

all: ifj 

ifj: main.o errors.o expressions.o functions.o parser.o scanner.o \
 stack.o statements.o strings.o symtable.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $<

errors.o: errors.c errors.h scanner.h strings.h
expressions.o: expressions.c errors.h expressions.h scanner.h strings.h \
 functions.h symtable.h parser.h statements.h stack.h
functions.o: functions.c errors.h expressions.h scanner.h strings.h \
 functions.h symtable.h parser.h statements.h
main.o: main.c errors.h parser.h scanner.h strings.h symtable.h
parser.o: parser.c errors.h functions.h symtable.h scanner.h strings.h \
 parser.h statements.h
scanner.o: scanner.c scanner.h strings.h errors.h
stack.o: stack.c errors.h stack.h scanner.h strings.h expressions.h
statements.o: statements.c errors.h expressions.h scanner.h strings.h \
 functions.h symtable.h parser.h statements.h
strings.o: strings.c strings.h
symtable.o: symtable.c symtable.h scanner.h strings.h errors.h

clean:
	rm -f ifj *.o

.PHONY: all clean
