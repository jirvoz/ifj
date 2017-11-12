#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include "scanner.h"

extern tToken last_token;

bool skip_statement();

bool parse();

#endif
