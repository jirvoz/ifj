#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdbool.h>

// Handle calling a function in IFJcode17
// name - name of the function to be called
bool call(char* name);

// Parse declaration of function
bool function_decl();

// Parse definition of function
bool function_def();

#endif
