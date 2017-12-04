#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include <stdbool.h>
#include "scanner.h"
#include "stack.h"

// Rows and cols in precedence table
typedef enum p_table_index
{
    EQ_IN,
    NOT_EQ_IN,
    LOWER_EQ_IN,
    HIGHER_EQ_IN,
    LOWER_IN,
    HIGHER_IN,
    PLUS_IN,
    MINUS_IN,
    MUL_IN,
    FLOAT_DIV_IN,
    INT_DIV_IN,
    LEFT_PARENTH_IN,
    RIGHT_PARENTH_IN,
    DOLAR_IN,

    // Only for internal communication in expressions.c, not included in precedence table
    INT_IN,                
    DOUBLE_IN,
    STRING_IN
} p_table_index;

// Structure sent to generateInstruction
// Data of stack
typedef struct tTerm
{
    enum p_table_index index;
    tToken token;
} tTerm;

// Parsing of expressions
// expected_type - type of variable, that should end on the top of stack
//                 can be INTEGER, DOUBLE, STRING, BOOLEAN
//                 or UNDEFINED_TOK = 0 (no specific type needed)
bool expression(token_type expected_type);

// Conversion from infix expression to postfix expression
//		expected_type - for checking correct data types in expression
// 		return_type   - for set and check correct data types if is expected_type
//						BOOLEAN UNDEFINED_TOK 
//		term 		  - type of variable, that contains index and token type 
//						of incoming token
//		stack 		  - stack for holding signs, for correct conversion to postfix
bool postfix(token_type expected_type, token_type return_type, struct tTerm term, struct tStack* stack);

// Verifying that variable or functions are defined and have correct data type
bool getTerm(tTerm* term);

// Checking priority between sign on stack and actual sign
bool getPriority (tTerm term, tStack* stack, token_type return_type);

// Generating instructions from postfix expression
bool generateInstruction(token_type return_type, tTerm sent_term);

#endif
