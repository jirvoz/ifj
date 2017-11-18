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
	MORE_IN,
	PLUS_IN,
	MINUS_IN,
	MUL_IN,
	FLOAT_DIV_IN,
	INT_DIV_IN,
	LEFT_PARENT_IN,
	RIGHT_PARENT_IN,
	INT_IN,					//13
	DOUBLE_IN,
	STRING_IN,
	EQ_COMMAND_IN,
	UNARY_MINUS_IN,
	DOLAR_IN
} p_table_index;

bool getTerm(tToken token, p_table_index* index)
{
	//token is identifier, figure out if var or func
	if (token.type == IDENTIFIER_TOK)
	{
		//search in func_table
		tSymbol* symbol = htSearch(func_table, token.attribute.string_ptr);

		if (symbol != NULL)
		{
			if (symbol->defined)
			{
				//token type 2 == p_table index 13 etc.
				if (symbol->type >= 2 && symbol->type <= 4)
				{
					*index = symbol->type + 11;
					return true;
				}
				addError(SEM_TYPE_ERROR, "Bad return type of function");
				return false;
			}
			addError(SEM_PROG_ERROR, "Undefined function");
			return false;
		}
		//search in var_table
		symbol = htSearch(var_table, token.attribute.string_ptr);

		if (symbol != NULL)
		{
			if (symbol->type >= 2 && symbol->type <= 4)
			{
				//token type 2 == p_table index 13 etc.
				*index = symbol->type + 11;
				return true;
			}
			addError(SEM_TYPE_ERROR, "Unknown variable type");
			return false;
		}
	}
	//operators
	else if(token.type >= 10 && token.type <= 22)
	{
		*index = token.type - 10;
	}
	//end of expression
	else
	{
		*index = DOLAR_IN;
	}
	return true;
}

bool expression(token_type expected_type)
{
    return true;
}
