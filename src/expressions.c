#include <stdio.h>
#include <malloc.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "stack.h"

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
	tList* list = getInfix(expected_type);
    *list = infixToPostfix(expected_type, list);

    generateInstructions(expected_type, list);
    return true;
}

tList* getInfix(token_type expected_type) 
{	
	tToken token;
	getNextToken(&token, stdin);

	tList* list_infix = malloc(sizeof(tList));

	p_table_index index;

	getTerm(token, &index);

	switch(expected_type)
	{
		case INTEGER_TOK:					// expected token is INTEGER or DOUBLE 
		case FLOATING_POINT_TOK:
		{
			listInit(list_infix);

			while (getTerm(token, &index) && index != DOLAR_IN)
			{
				if(index == INT_IN || index == DOUBLE_IN || index == PLUS_IN || index == MINUS_IN || index == MUL_IN || 	//Operands and operations allowed when expected type is INTEGER or DOUBLE
				   index == FLOAT_DIV_IN || index == INT_DIV_IN || index == LEFT_PARENT_IN || index == RIGHT_PARENT_IN)
				{
    				listInsertLast(list_infix, token, index);
    				getNextToken(&token, stdin);		
				}
				else
				{
					addError(SEM_TYPE_ERROR,"Bad operation or operand in expression");
					listFree(list_infix);
				}
			}

			if(index != DOUBLE_IN)
			{
				listFree(list_infix);
			}

			return list_infix; 
		}
		case STRING_TOK:
		{
			while (getTerm(token, &index) && index != DOLAR_IN)
			{
				if(index == STRING_IN || index == PLUS_IN || index == DOLAR_IN)
				{
					listInsertLast(list_infix, token, index);
					//concatenate
    				getNextToken(&token, stdin);		
				}
				else
				{
					addError(SEM_TYPE_ERROR,"Bad operation or operand in expression");
					listFree(list_infix);
				}
			}

			if (index != DOLAR_IN)
			{
				listFree(list_infix);
			}

			return list_infix; 
		}

		case BOOLEAN:
		{
			while (getTerm(token, &index) && index != DOLAR_IN)
			{
				listInsertLast(list_infix, token, index);
    			getNextToken(&token, stdin);		
			}

			if (index != DOLAR_IN)
			{
				listFree(list_infix);
			}

			return list_infix; 
		}
        default:
            listFree(list_infix);
            addError(OTHER_ERROR, "Unknown error");
            return list_infix;
	}	
}


/*
tList infixToPostfix(token_type expected_type, tList* list_infix)
{
	tList list_postfix;

	tStack stack;
	stackInit(&stack);
    return list_postfix;
}
*/

//this global variable will be used if expected type is STRING
bool string_flag;

bool generateInstruction(token_type expected_type, tTerm term)
{
    //prepare string variables in Local Frame
    if (string_flag == false && expected_type == STRING_TOK)
    {
        printf("DEFVAR LF@$tmp_string1\n");
        printf("DEFVAR LF@$tmp_string2\n");
        printf("DEFVAR LF@$tmp_string3\n");
    }

    //main switch
    switch (term->index)
    {
        //push integer to stack
        case INT_IN:
        {
            //constant
            if (term->token.type == INTEGER_TOK)
                printf("PUSHS int@%d\n", term->token.attribute.number);
            //identifier
            else
                printf("PUSHS LF@%s\n", term->token.attribute.string_ptr);
            //convert to float
            printf("INT2FLOAT\n");
        }
            break;
        //push float to stack
        case DOUBLE_IN:
        {
            //constant
            if (term->token.type == FLOATING_POINT_TOK)
                printf("PUSHS float@%g\n", term->token.attribute.float_number);
            //identifier
            else
                printf("PUSHS LF@%s\n", term->token.attribute.string_ptr);
        }
            break;
        //ADDS instruction or STRING CONCANTENATION
        case PLUS_IN:
        {
            //if int or float, add
            if (expected_type != STRING_TOK)
                printf("ADDS\n");

            //if string, concatenate
            else
            {
                printf("CONCAT LF@$tmp_string3 LF@$tmp_string1 LF@$tmp_string2\n");
                printf("MOVE LF@$tmp_string1 LF@$tmp_string3\n");
            }   
        }
            break;
        //SUBS instruction
        case MINUS_IN:
        {
            printf("SUBS\n");
        }
            break;
        //MULS instruction
        case MUL_IN:
        {
            printf("MULS\n");
        }
            break;
        //classic DIVS
        case FLOAT_DIV_IN:
        {
            printf("SUBS\n");
        }
            break;
        //'\' DIVS - integers
        case INT_DIV_IN:
        {
            printf("SUBS\n");
            printf("FLOAT2INTS\n");
            printf("INT2FLOATS\n");
        }
            break;
        //strings
        case STRING_IN:
        {
            if (term->token.type == STRING_TOK)
            {
                if (string_flag)
                    printf("MOVE LF@$tmp_string2 string@%s\n", term->token.attribute.string_ptr);
                else
                {
                    printf("MOVE LF@$tmp_string1 string@%s\n", term->token.attribute.string_ptr);
                    string_flag = true;
                } 
            }
            else if (term->token.type == IDENTIFIER_TOK)
            {
                if (string_flag)
                    printf("MOVE LF@$tmp_string2 string@%s\n", term->token.attribute.string_ptr);
                else
                {
                    printf("MOVE LF@$tmp_string1 GF@%s\n", term->token.attribute.string_ptr);
                    string_flag = true;
                }
            }
        }
            break;
        //'<'- comparison by LTS instruction - automatically pops flag to stack
        case LESS_IN:
        {
            printf("LTS\n");
        }
            break;
        //'>' - comparison by GTS instruction - automatically pops flag to stack
        case MORE_IN:
        {
            printf("GTS\n");
        }
            break;
        // '<=' - it's necessary to use also ORS instructions
        case LESS_EQ_IN:
        {
            printf("LTS\n");
            printf("DEFVAR LF@$flag\n");
            printf("POPS LF@$flag\n");
            printf("EQS\n");
            printf("PUSHS LF@$flag\n");
            printf("ORS\n");
        }
            break;
        //'>=' - it's necessary to use also ORS instructions
        case MORE_EQ_IN:
        {
            printf("GTS\n");
            printf("DEFVAR LF@$flag\n");
            printf("POPS LF@$flag\n");
            printf("EQS\n");
            printf("PUSHS LF@$flag\n");
            printf("ORS\n");  
        }
            break;
        //'=' - use simple EQS instruction
        case EQ_EXPR_IN:
        {
            printf("EQS\n"); 
        }
            break;
        case
        default:
        {
            addError(OTHER_ERROR, NULL);
        }
    }
}
    //convert to expected type
    switch (expected_type)
    {
        case INTEGER_TOK:
        {
            printf("FLOAT2R2EINTS LF\n");
        }
            break;
        case BOOLEAN:
        {
            if (final_bool)
            {
                printf("PUSHS GF@bool@true%s\n");  
            }
            else
            {
                printf("PUSHS GF@bool@false%s\n"); 
            }
            
        }
            break;
}