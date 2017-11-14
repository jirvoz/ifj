#include <stdio.h>
#include <malloc.h>
#include "errors.h"

unsigned error_count = 0;
int exit_code = 0;
tCode_pair* errors;

char* err_message[] =
{
    "Lexical error",
    "Syntax error",      
    "Semantic error in the program",
    "Type incompatibility semantic error",
    "Else semantic error",
    "Internal error"
}; 

//function counts errors and creates array of errors
void addError (unsigned line, err_code code)
{
    //exit code will be first error code
    if (exit_code == 0)
    {
        exit_code = code;
    }

    if (error_count == 0)
    {
        errors = malloc(sizeof(tCode_pair));
    }
    else
    {
        errors = realloc(errors, error_count + 1);
    }

    //if memory error, do nothing
    if (errors == NULL)
    {
        return;
    }

    //add new error to array
    errors[error_count].line = line;
    errors[error_count].code = code;
    error_count++;
}

void printErrors()
{
    for (int i = 0; i < error_count; i++)
    {
        if (errors[i].code == 6)
        {
            fprintf(stderr, err_message[4]);
        }
        else if (errors[i].code == 99)
        {
            fprintf(stderr, err_message[5]);
        }
        else
        {
            fprintf(stderr, err_message[errors[i].code - 1]);
        }
        fprintf(stderr, " on the line: %u\n", errors[i].line);
    }
    free(errors);
}
