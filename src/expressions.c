#include <stdio.h>
#include <malloc.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"

/*****************************LIST-STUFF**************************/

//list initialization
void listInit(tList* list)
{
    list->first  = NULL;
    list->last   = NULL;
    list->active = NULL;
}
  
//list dealocation
void listFree(tList* list)
{
    tList_item* ptr;
    while (list->first != NULL)
    {
        ptr = list->first;
        list->first = list->first->next;

        //free whole item
        free(ptr);
    }
}

//insert new item to end of the list
void listInsertLast(tList* list, tToken token, p_table_index index)
{
    tList_item *new_item;
    new_item = malloc(sizeof (tList_item));
    new_item->token = token;
    new_item->index = index;
    new_item->next = NULL;

    if (list->first == NULL)
    {
        list->first = new_item;
    }
    else
    {
        list->last->next = new_item;
    }
    list->last = new_item;
}

//activate first item
void listFirst(tList* list)
{
    list->active = list->first;
}

//activate item after actual activated item
void listNext(tList* list)
{
    if (list->active != NULL)
    {
        list->active = list->active->next;   
    }
}

//activate token on pointer
void listGoto(tList* list, tList_item* item)
{
    list->active = item;
}

//return pointer of last item 
tList_item* listGetPointerLast(tList* list)
{
    return list->last;
}

//return active item
tList_item* listGetData(tList* list)
{
    if (list->active == NULL)
    {
        addError(OTHER_ERROR, "Internal error");
        return NULL;
    }
    else
    {
        return list->active;  
    } 
}

/*************************END-OF-LIST-STUFF******************************/


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

/*
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
*/
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
	tList* list_infix = getInfix(expected_type);
    //will be deleted, just for compilation
    list_infix = (void*) list_infix;
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

/*tList infixToPostfix(token_type expected_type, tList list_infix)
{
	tList list_postfix;

	tStack stack;
	stackInit(&stack);
}*/