#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"

// Shortcut for getting next token
#define UPDATE_LAST_TOKEN(); if (getNextToken(&last_token, stdin) == false) return false;

// Shortcut for printig error and returning from function
#define ERROR_AND_RETURN(err_code, ...) do { addError(err_code, __VA_ARGS__); \
                                             return false; } while (0)

// Last loaded token for public use
extern tToken last_token;

// Table of declared/defined functions
extern tHtable* func_table;

// Table of defined variables
extern tHtable* var_table;


// Temporary function that eats tokens till EOL, END, EOF
bool skip_statement();

// Crossroads for parsing single statements
// (variable definition, assingnment, if, while,...)
bool statement();

// Parsing of set of statements ended by END token
bool statement_list();

// Parsing crossroads for functions and main scope
bool program();

// Main function for eating IFJ17 tokens and forging IFJcode17
bool parse();

#endif
