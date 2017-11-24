#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdbool.h>
#include "symtable.h"

// Name of function in which program currently is
// is NULL when out of function
extern char* actual_function;

// Handle calling a function in IFJcode17
// name - name of the function to be called
bool call(char* name);

// Parse declaration of function
bool function_decl();

// Parse definition of function
bool function_def();

// Inbuild functions

//function gets int and on stack returns chr from ASCII table
bool callAsc();

//function gets int and on stack returns char related to ints position in ASCII
bool callChr();

//get string and on stack returns number of chars in it
//if function succeded retrun "true" else "false"
bool callLength();

//func gets one string and two ints. First int is start position in string
//second is length of substring which i return to top of stack
bool callSubstr();

#endif
