#include "strings.h"

#define ERROR 1;    // return value when it was error
#define SUCCES 0;   // return value when it was success

#define STR_LEN_INC 8; // the basic size of the allocated space for the string, relocating will be multiples of this number

int stringInit(strings *str_1)  // function initialize new string
{
    str_1->string = (char*) malloc(sizeof(char));
    
    if (str_1->string != NULL)
    {
        str_1->string[0] = '\0';
        str_1->length = 0;
        str_1->allocatedSize = sizeof(char);
       
        return SUCCESS;
    }

    else
       return ERROR;
}

void stringFree(strings *str_1) // function to free the string from memory
{
    free(str_1->string);
}

void stringClear(string *str_1) // function to clear content of the string
{
   str_1->string[0] = '\0';
   str_1->length = 0;
}

int stringAddChar(strings *str_1, char c) // function to add new char to the end of string
{
    if (str_1->length + 1 >= s1->allocatedSize)
    {
        str_1->string = (char*) realloc(str_1->string, (str_1->length + sizeof(char)) )
        if (str_1->string != NULL)
        {
            str_1->allocatedSize = str_1->length + sizeof(char);
        }
        else
            return ERROR;   
    }
    
    str_1->string[str_1->length] = c;
    str_1->length++;
    str_1->string[str_1->length] = '\0';
    
    return SUCCESS;
}

