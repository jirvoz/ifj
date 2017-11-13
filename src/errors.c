#include "errors.h"
#include <stdio.h>

unsigned error_count = 0;
int exit_code = 0;

char* err_message[] = 
{
    "lexical error\n",
    "Syntax error\n",      
    "Semantic error - in program\n",
    "Semantic error - type incompatibility\n",
    "Semantic error - else\n",
    "Internal error\n"
}; 

void addError (unsigned line, err_code code) {
    if (code == 6) {
        fprintf(stderr, err_message[4]);
    }
    else if (code == 99) {
        fprintf(stderr, err_message[5]);
       
    }
    else {
        fprintf(stderr, err_message[code - 1]);
    }
    fprintf(stderr, "Řádek: %u\n", line); 
    error_count++;
}

