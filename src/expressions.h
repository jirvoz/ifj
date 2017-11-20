#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include <stdbool.h>
#include "scanner.h"
#include "stack.h"

//rows and cols in precedence table
typedef enum p_table_index
{
    EQ_EXPR_IN,
    NOT_EQ_IN,
    LESS_EQ_IN,
    MORE_EQ_IN,
    LESS_IN,
    MORE_IN,
    PLUS_IN,
    MINUS_IN,
    MUL_IN,
    FLOAT_DIV_IN,
    INT_DIV_IN,
    LEFT_PARENT_IN,
    RIGHT_PARENT_IN,
    INT_IN,                 //13
    DOUBLE_IN,
    STRING_IN,
    EQ_COMMAND_IN,
    UNARY_MINUS_IN,
    FUNCTION_IN,
    DOLAR_IN
} p_table_index;

//structure sent to generateInstruction
//data of stack
typedef struct tTerm
{
    tToken token;
    p_table_index index;
} tTerm;

// Parsing of expressions
// expected_type - type of variable, that should end on the top of stack
//                 can be INTEGER, DOUBLE, STRING, BOOL
//                 or UNDEFINED_TOK = 0 (no specific type needed)
bool expression(token_type expected_type);

bool getTerm(tToken token, p_table_index* index);
bool generateInstruction(token_type expected_type, tTerm term);

#endif
