//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        errors.h
//  Description: Header file of error module
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00


#ifndef _ERRORS_H_
#define _ERRORS_H_

extern int exit_code;

// This structure represent one error message
typedef enum err_code 
{
    NO_ERROR = 0,
    LEX_ERROR = 1,
    SYN_ERROR = 2,
    SEM_PROG_ERROR = 3,
    SEM_TYPE_ERROR = 4,
    SEM_OTHER_ERROR = 6,
    OTHER_ERROR = 99
} err_code;

// Print error message and set return value
// Return value is error code of first error
//
// code is error type and possible return value
// fmt and other arguments are same as in printf
void addError(err_code code, const char* fmt, ...);

#endif
