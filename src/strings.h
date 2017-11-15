#ifndef _STRINGS_H_
#define _STRINGS_H_

typedef struct
{
    char* str;            // loaded string ended with the '\0'
    int length;           // length of string
    int allocatedSize;    // size of allocated memory for string
} string;

int stringInit(string*);
void stringFree(string*);
void stringClear(string*);

int stringAddChar(char, string*);
int stringConcat(const char*, string*);

#endif
