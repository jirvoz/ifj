#include "strings.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define STR_LEN_INC 8           //the basic size of the allocated space for the str, relocating will be multiples of this number

int stringInit(string* str_1)   //function initialize new str
{
    str_1->str = malloc(STR_LEN_INC);

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

void stringFree(string *str_1)  //function to free the str from memory
{
    free(str_1->str);
}

void stringClear(string *str_1) //function to clear content of the str
{
   str_1->str[0] = '\0';
   str_1->length = 0;
}

int stringAddChar(char c, string *str_1) //function to add new char to the end of str
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

int stringConcat(const char* str_2, string* str_1)
{
    int str_2_length = strlen(str_2);

    if ((str_2_length  + str_1->length + 1) >= str_1->allocatedSize)
    {
        str_1->str = (char*) realloc(str_1->str, (str_1->length + str_2_length + STR_LEN_INC));

        if (str_1->str != NULL)
            str_1->allocatedSize = str_1->length + str_2_length + STR_LEN_INC;
        else
            return false;
    }

    strcat(str_1->str, str_2);
    str_1->length = str_1->length + str_2_length;
    return true;
}
