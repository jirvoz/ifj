//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        parser.h
//  Description: Header file of top parser logic
//               Starting and generic rules for syntax analysis
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"

// Shortcut for getting next token
#define UPDATE_LAST_TOKEN() do { if (!getNextToken(&last_token, stdin)) \
                                     return false; } while (0)

// Shortcut for printig error and returning from function
#define ERROR_AND_RETURN(err_code, ...) do { addError(err_code, __VA_ARGS__); \
                                             return false; } while (0)

// Last loaded token for public use
extern tToken last_token;

// Table of declared/defined functions
extern tHTable* func_table;

// Table of defined variables
extern tHTable* var_table;


// Temporary function that eats tokens till EOL, END, EOF
bool skip_statement();

// Crossroads for parsing single statements
// (variable definition, assingnment, if, while,...)
bool statement();

// Parsing of set of statements ended by END token
bool statementList();

// Parsing crossroads for functions and main scope
bool program();

// Main function for eating IFJ17 tokens and forging IFJcode17
bool parse();

#endif
