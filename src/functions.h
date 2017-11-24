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
bool callAsc();
bool callChr();
bool callLength();
bool callSubstr();

#endif
