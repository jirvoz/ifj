//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        strings.c
//  Description: Source file of String module
//               Struct is data structure representing dynamically allocated 
//               array of chars, size of allocated memmory and length of string
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "strings.h"

// The basic size of the allocated space for the str, relocating will be multiples of this number
#define STR_LEN_INC 8

// Initialize new str, allocated size is STR_LEN_INC
int stringInit(string* str_1)   
{
    str_1->str = (char*) malloc(STR_LEN_INC);

    if (str_1->str != NULL)
    {
        str_1->str[0] = '\0';
        str_1->length = 0;
        str_1->allocatedSize = STR_LEN_INC;
       
        return true;
    }

    else
       return false;
}

// Free the str allocated memory
void stringFree(string *str_1)
{
    free(str_1->str);
}

// Clear content of the str
void stringClear(string *str_1)
{
   str_1->str[0] = '\0';
   str_1->length = 0;
}

// Add new char to the end of str
int stringAddChar(char c, string *str_1)
{
    if (str_1->length + 1 >= str_1->allocatedSize)
    {
        str_1->str = (char*) realloc(str_1->str, (str_1->length + STR_LEN_INC));
        if (str_1->str != NULL)
            str_1->allocatedSize = str_1->length + STR_LEN_INC;
        else
            return false;  
    }
    
    str_1->str[str_1->length] = c;
    str_1->length++;
    str_1->str[str_1->length] = '\0';
    return true;
}

// Allocate memmory and concatenate string
int stringConcat(const char* str_2, string* str_1)
{
    int str_2_length = strlen(str_2);

    if (str_2_length  + str_1->length >= str_1->allocatedSize)
    {
        // Don't forget the null byte at the end (len1 + len2 + 1)
        str_1->str = (char*) realloc(str_1->str, (str_1->length + str_2_length + 1));

        if (str_1->str != NULL)
            str_1->allocatedSize = str_2_length  + str_1->length + 1;
        else
            return false;
    }
    strcat(str_1->str, str_2);
    str_1->length = str_1->length + str_2_length;

    return true;
}
