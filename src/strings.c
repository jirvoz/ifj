#include "strings.h"
#include "stdlib.h"

#define ERROR 1    // return value when it was error
#define SUCCESS 0   // return value when it was success

#define STR_LEN_INC 8 // the basic size of the allocated space for the str, relocating will be multiples of this number

int stringInit(string* str_1)  // function initialize new str
{
    str_1->str = (char*) malloc(STR_LEN_INC);

    if (str_1->str != NULL)
    {
        str_1->str[0] = '\0';
        str_1->length = 0;
        str_1->allocatedSize = STR_LEN_INC;
       
        return SUCCESS;
    }

    else
       return ERROR;
}

void stringFree(string *str_1) // function to free the str from memory
{
    free(str_1->str);
}

void stringClear(string *str_1) // function to clear content of the str
{
   str_1->str[0] = '\0';
   str_1->length = 0;
}

int stringAddChar(char c, string *str_1) // function to add new char to the end of str
{
    if (str_1->length + 1 >= str_1->allocatedSize)
    {
        str_1->str = (char*) realloc(str_1->str, (str_1->length + STR_LEN_INC) );
        if (str_1->str != NULL)
        {
            str_1->allocatedSize = str_1->length + STR_LEN_INC ;
        }
        else
        {
            return ERROR;  
        }
              
    }
    
    str_1->str[str_1->length] = c;
    str_1->length++;
    str_1->str[str_1->length] = '\0';
    
    return SUCCESS;
}

void addError(int line, int errors)
{
    return;
}

int identifierTest(string* neco, char** neco_druhe)
{
    return -1;
}

