#ifndef _STRINGS_H_
#define _STRINGS_H_

typedef struct
{
    char* str;            // loaded string ended with the '\0'
    int length;           // length of string
    int allocatedSize;    // size of allocated memory for string
} string;

int stringInit(string* str_1);
void stringFree(string* str_1);
void stringClear(string* str_1);

int stringAddChar(char c, string* str_1);
int stringConcat(const char* str_2, string* str_1);

#endif
