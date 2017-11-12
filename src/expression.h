#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <stdbool.h>
#include "scanner.h"

// Parsing of expressions
// expected_type - type of variable, that ends on the top of stack
//                 can be INTEGER, DOUBLE, STRING or BOOL
bool expression(token_type expected_type);

#endif
