#ifndef _ERRORS_H_
#define _ERRORS_H_

#define SUCCESS 0               //bash style
#define FAILURE 1               //bash style

extern int exit_code;

//this structure represent one error message

typedef enum err_code 
{
    LEX_ERROR = 1,
    SYN_ERROR = 2,
    SEM_PROG_ERROR = 3,
    SEM_TYPE_ERROR = 4,
    SEM_OTHER_ERROR = 6,
    OTHER_ERROR = 99
} err_code;

typedef struct tCode_pair
{
    unsigned line;
    err_code code;
} tCode_pair;

void addError(unsigned line, err_code code);
void printErrors();

#endif
