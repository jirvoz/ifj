#include <stdio.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "postfix_list.h"
#include "statements.h"

//size of precedence table
#define P_TAB_SIZE 19

const int precedence_table[P_TAB_SIZE][P_TAB_SIZE] =
{
//exp'=' '<>' '<=' '>='  '<'  '>'  '+'  '-'  '*'  '/'  '\'  '('  ')' 'int''dob''str' com'=' un'-'  '$'
    {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //exp'='
    {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //'<>'
    {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //'<='
    {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //'>='
    {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //'<'
    {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //'>'
    {'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '<', '<', '<',    'x',  '<', '>'}, //'+'
    {'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '<', '<', 'x',    'x',  '<', '>'}, //'-'
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', 'x',    'x',  '<', '>'}, //'*'
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', 'x',    'x',  '<', '>'}, //'/'
    {'>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '>', '<', 'x', 'x',    'x',  '<', '>'}, //'\'
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<',    'x',  '<', 'x'}, //'('
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', 'x', 'x', 'x',    '>',  'x', '>'}, //')'
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', 'x', 'x', 'x',    '=',  'x', '>'}, //'int'
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', 'x', '>', 'x', 'x', 'x',    '=',  'x', '>'}, //'dob'
    {'>', '>', '>', '>', '>', '>', '>', 'x', 'x', 'x', 'x', 'x', '>', 'x', 'x', 'x',    '=',  'x', '>'}, //'str'
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', '<', '<', '<',    'x',  '<', '>'}, //com'='
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', 'x', '<', '<', 'x',    'x',  '>', '>'}, //un'-'
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', '<', '<', '<',    'x',  '<', 'x'}, //'$'
};

//number of rules
#define RULE_COUNT 10


//this function compare two rules
bool compareRules(int* rule1, int* rule2)
{
    for (int i = 0; i < 3; i++)
    {
        if (rule1[i] != rule2[i])
        {
            return false;
        }
    }
    return true;
}

//this functions returns number of rule 
//if unsuccess, returns -1
int generateRule(int *rule)
{

    for (int i = 0; i < RULE_COUNT, i++)
    {
        if (compareRules(rule, rules[i]))
        {
            return i;
        }
    }
    return -1;
}

//declarations of variable in symboltable
void 

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
	tList list_infix = getInfix(expected_type);

    //return true;
}

tList getInfix(token_type expected_type) 
{	
	tToken token;
	getNextToken(&token, stdin);

	tList list_infix;

	p_table_index index;

	getTerm(token, &index);

	switch(expected_type)
	{
		case INTEGER_TOK:					// expected token is INTEGER or DOUBLE 
		case FLOATING_POINT_TOK:
		{
			listInit(&list_infix);

			while (getTerm(token, &index) && index != DOLAR_IN) ;
			{
				if(index == INT_IN || index == DOUBLE_IN || index == PLUS_IN || index == MINUS_IN || index == MUL_IN || 	//Operands and operations allowed when expected type is INTEGER or DOUBLE
				   index == FLOAT_DIV_IN || index == INT_DIV_IN || index == LEFT_PARENT_IN || index == RIGHT_PARENT_IN)
				{
    				listInsertLast(&list_infix, token);
    				getNextToken(&token, stdin);		
				}
				else
				{
					addError(SEM_TYPE_ERROR,"Bad operation or operand in expression");
					listFree(&list_infix);
				}
			}

			if(index != DOUBLE_IN)
			{
				listFree(&list_infix);
			}

			return list_infix; 
		}
		case STRING_TOK:
		{
			while (getTerm(token, &index) && index != DOLAR_IN);
			{
				if(index == STRING_IN || index == PLUS_IN || index == DOLAR_IN)
				{
					listInsertLast(&list_infix, token);
					//concatenate
    				getNextToken(&token, stdin);		
				}
				else
				{
					addError(SEM_TYPE_ERROR,"Bad operation or operand in expression");
					listFree(&list_infix);
				}
			}

			if (index != DOLAR_IN)
			{
				listFree(&list_infix);
			}

			return list_infix; 
		}

		case BOOLEAN:
		{
			while (getTerm(token, &index) && index != DOLAR_IN);
			{
				listInsertLast(&list_infix, token);
    			getNextToken(&token, stdin);		
			}

			if (index != DOLAR_IN)
			{
				listFree(&list_infix);
			}

			return list_infix; 
		}	
	}	
}

/*tList infixToPostfix(token_type expected_type, tList list_infix)
{
	tList list_postfix;

	tStack stack;
	stackInit(&stack);
}*/