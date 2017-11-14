#include <stdio.h>
#include <stdarg.h>
#include "errors.h"
#include "scanner.h"

int exit_code = 0;

char* err_messages[] =
{                                           // return values:
    "Info",                                 // 0
    "Lexical error",                        // 1
    "Syntax error",                         // 2
    "Semantic error in the program",        // 3
    "Type incompatibility semantic error",  // 4
    "Else semantic error",                  // 6
    "Internal error"                        // 99
}; 

//function counts errors and creates array of errors
void addError(err_code code, const char* fmt, ...)
{
    //exit code will be first error code
    if (exit_code == 0)
        exit_code = code;

    // print error type
    fprintf(stderr, "Line %d: ", line);
    if (code == 99)
        fprintf(stderr, "%s: ", err_messages[6]);
    else if (code == 6)
        fprintf(stderr, "%s: ", err_messages[5]);
    else
        fprintf(stderr, "%s: ", err_messages[code]);

    // print custom message
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}
