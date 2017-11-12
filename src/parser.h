#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include "scanner.h"

// Shortcut for getting next token
#define UPDATE_LAST_TOKEN(); if (getNextToken(&last_token, stdin) != SUCCESS) return false;


// Last loaded token for public use
extern tToken last_token;


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
