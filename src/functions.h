//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        statements.h
//  Description: Header file of function parsing
//               Rules for parsing functions
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

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

#endif
