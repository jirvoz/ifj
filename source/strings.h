#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>


typedef struct
{
  char* string;		// loaded string ended with the '\0'
  int length;		// length of string
  int allocatedSize;	// size of allocated memory for string
} strings;

int stringInit(strings *str_1);
void stringFree(strings *str_1);
void stringClear(string *str_1);

int stringAddChar(strings *str_1, char c);

#endif