#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include "scanner.h"

#define UPDATE_LAST_TOKEN(); if (getNextToken(&last_token, stdin) != SUCCESS) return false;

extern tToken last_token;

bool skip_statement();

bool statement_list();

bool parse();

#endif
