#include <stdio.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"

//rows and cols in precedence table
typedef enum p_table_index
{
	EQ_EXPR_IN,
	NOT_EQ_IN,
	LESS_EQ_IN,
	MORE_EQ_IN,
	LESS_IN,
	PLUS_IN,
	MINUS_IN,
	MUL_IN,
	FLOAT_DIV_IN,
	INT_DIV_IN,
	EQ_COMMAND_IN,
	UNARY_MINUS_IN,
	INT_IN,
	DOUBLE_IN,
	STRING_IN,
	LEFT_PARENT_IN,
	RIGHT_PARENT_IN,
	DOLAR_IN
}

bool expression(token_type expected_type)
{
    return true;
}
