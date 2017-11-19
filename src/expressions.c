#include <stdio.h>
#include <malloc.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "stack.h"

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
    else if (token.type >= 10 && token.type <= 22)
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
    *list = infixToPostfix(list);
    
    return generateInstructions(expected_type, list);
}

tList* getInfix(token_type expected_type) 
{   
    tToken token;
    getNextToken(&token, stdin);

    tList* list_infix = malloc(sizeof(tList));

    p_table_index index;

    int operand_count = 0;
    int operation_count = 0;
    int left_parent_count = 0;
    int right_parent_count = 0;

    getTerm(token, &index);

    switch(expected_type)
    {
        case INTEGER_TOK:                   // expected token is INTEGER or DOUBLE 
        case FLOATING_POINT_TOK:
        {
            listInit(list_infix);

            while (getTerm(token, &index) && index != DOLAR_IN)
            {
                if (index == INT_IN || index == DOUBLE_IN || index == PLUS_IN || index == MINUS_IN || index == MUL_IN ||     //Operands and operations allowed when expected type is INTEGER or DOUBLE
                   index == FLOAT_DIV_IN || index == INT_DIV_IN || index == LEFT_PARENT_IN || index == RIGHT_PARENT_IN)
                {
                    if (index == INT_IN || index == DOUBLE_IN)
                    {
                        operand_count++;
                    }
                    else if (index = LEFT_PARENT_IN)
                    {
                        left_parent_count++;
                    }
                    else if (index == RIGHT_PARENT_IN)
                    {
                        right_parent_count++;
                    }
                    else 
                    {
                        operation_count++;
                    }

                    listInsertLast(list_infix, token, index);
                    getNextToken(&token, stdin);        
                }
                else
                {
                    addError(SEM_TYPE_ERROR,"Bad operation or operand in expression");          //not expected operands or operations
                    listFree(list_infix);
                    return list_infix;
                }
            }

            if (right_parent_count == left_parent_count)
            {
                if ((operation_count + 1)== operand_count)
                {
                    return list_infix;    
                }
                else
                {
                    addError(SEM_TYPE_ERROR,"Bad number of operations or operands in expression");          //not expected operands or operations
                    listFree(list_infix);
                    return list_infix;
                }
            }
            else
            {
                addError(SEM_TYPE_ERROR,"Bad number of parentals in expression");           //not expected number of parentals
                listFree(list_infix);
                return list_infix;
            }
             
        }

        case STRING_TOK:
        {
            while (getTerm(token, &index) && index != DOLAR_IN)
            {
                if (index == STRING_IN || index == PLUS_IN || index == LEFT_PARENT_IN || index == RIGHT_PARENT_IN)
                {
                    if (index == STRING_IN)
                    {
                        operand_count++;
                    }
                    else if (index == RIGHT_PARENT_IN)
                    {
                        right_parent_count++;
                    }
                    else if (index == LEFT_PARENT_IN)
                    {
                        left_parent_count++;
                    }
                    else
                    {
                        operation_count++;
                    }

                    listInsertLast(list_infix, token, index);
                    //concatenate
                    getNextToken(&token, stdin);        
                }
                else
                {
                    addError(SEM_TYPE_ERROR,"Bad operation or operand in expression");
                    listFree(list_infix);
                    return list_infix; 
                }
            }

            if (right_parent_count == left_parent_count)
            {
                if ((operation_count + 1)== operand_count)
                {
                    return list_infix;    
                }
                else
                {
                    addError(SEM_TYPE_ERROR,"Bad number of operations or operands in expression");          //not expected operands or operations
                    listFree(list_infix);
                    return list_infix;
                }
            }
            else
            {
                addError(SEM_TYPE_ERROR,"Bad number of parentals in expression");          //not expected number of parentals
                listFree(list_infix);
                return list_infix;
            } 
        }

        case BOOLEAN:
        {
            while (getTerm(token, &index) && index != DOLAR_IN)
            {
                if (index == STRING_IN || index == DOUBLE_IN || index == INT_IN)
                {
                    operand_count++;
                }
                else if ( index == LEFT_PARENT_IN)
                {
                    left_parent_count++;
                }
                else if (index == RIGHT_PARENT_IN)
                {
                    right_parent_count++;
                }
                else
                {
                    operation_count++;
                }

                listInsertLast(list_infix, token, index);
                getNextToken(&token, stdin);        
            }

            if (right_parent_count == left_parent_count)
            {
                if ((operation_count + 1)== operand_count)
                {
                    return list_infix;    
                }
                else
                {
                    addError(SEM_TYPE_ERROR,"Bad number of operations or operands in expression");          //not expected operands or operations
                    listFree(list_infix);
                    return list_infix;
                }
            }
            else
            {
                addError(SEM_TYPE_ERROR,"Bad number of parentals in expression");           //not expected number of parentals
                listFree(list_infix);
                return list_infix;
            }
        }
        default:
            listFree(list_infix);
            addError(OTHER_ERROR, "Unknown error");
            return list_infix;
    }   
}

/****************************************************************************/

tList* infixToPostfix(tList* list_infix)
{
    tList* list_postfix = NULL; //pointer to postfix_list

    if (list_infix->first != NULL)  //if infix_list is empty return NULL
    {
        tStack stack;               //stack init for operators
        stackInit(&stack);

        tList_item* item;

        tList* list_postfix = malloc(sizeof(tList));
        listFirst(list_infix);  //activate first item in list

        while ((item = listGetData(list_infix)) != NULL)
        {

        }








    }
    return list_postfix;
}

void generateInstructions(token_type expected_type, tList* list)
{
    listFirst(list);
    tList_item* item;
    bool string_flag = false;

    //prepare string variables in Local Frame
    if (expected_type == STRING_TOK)
    {
        printf("DEFVAR LF@tmp_string1\n");
        printf("DEFVAR LF@tmp_string2\n");
        printf("DEFVAR LF@tmp_string3\n");
    }

    while ((item = listGetData(list)) != NULL)
    {
        switch (item->index)
        {
            //push integer to stack
            case INT_IN:
            {
                if (item->token.type == INTEGER_TOK)
                {
                    printf("PUSHS LF@int@%d\n", item->token.attribute.number);
                    printf("INT2FLOAT LF\n");
                }
                else if (item->token.type == IDENTIFIER_TOK)
                {
                    printf("DEFVAR LF@%s\n", item->token.attribute.string_ptr);
                    printf("MOVE LF@%s GF@v%s\n", item->token.attribute.string_ptr, item->token.attribute.string_ptr);
                    printf("PUSHS LF@%s\n", item->token.attribute.string_ptr);
                    printf("INT2FLOAT LF\n");
                }
            }
            //push float to stack
            case DOUBLE_IN:
            {
                if (item->token.type == FLOATING_POINT_TOK)
                {
                    printf("PUSHS LF@float@%g\n", item->token.attribute.float_number);
                }
                else if (item->token.type == IDENTIFIER_TOK)
                {
                    printf("DEFVAR LF@%s\n", item->token.attribute.string_ptr);
                    printf("MOVE LF@%s GF@v%s\n", item->token.attribute.string_ptr, item->token.attribute.string_ptr);
                    printf("PUSHS LF@%s\n", item->token.attribute.string_ptr);
                }
            }
            //ADDS instruction
            case PLUS_IN:
            {
                printf("ADDS LF\n");
            }
            //SUBS instruction
            case MINUS_IN:
            {
                printf("SUBS LF\n");
            }
            //MULS instruction
            case MUL_IN:
            {
                printf("MULS LF\n");
            }
            //classic DIVS
            case FLOAT_DIV_IN:
            {
                printf("SUBS LF\n");
            }
            //'\' DIVS - integers
            case INT_DIV_IN:
            {
                printf("SUBS LF\n");
                printf("FLOAT2R2EINTS LF\n");
                printf("INT2FLOAT LF\n");
            }
            //strings
            case STRING_IN:
            {
                if (item->token.type == STRING_TOK)
                {
                    if (string_flag)
                    {
                        printf("DEFVAR LF@tmp_string1\n");
                        printf("POPS LF@tmp_string1\n");
                        printf("DEFVAR LF@tmp_string2\n");
                        printf("MOVE LF@tmp_string2 string@%s\n", item->token.attribute.string_ptr);
                        printf("DEFVAR LF@tmp_string3\n");
                        printf("CONCAT LF@tmp_string3 LF@tmp_string1 LF@tmp_string2\n");
                        printf("PUSHS LF@tmp_string3\n");

                        if (item->next.index == PLUS_IN)
                        {
                            item = item->next;
                        }
                    }
                    else
                    {
                        printf("PUSHS LF@string@%s\n", item->token.attribute.string_ptr);
                        string_flag = true;
                    }
                    
                }
                else if (item->token.type == IDENTIFIER_TOK)
                {
                    if (string_flag)
                    {
                        printf("DEFVAR LF@tmp_string1\n");
                        printf("POPS LF@tmp_string1\n");
                        printf("DEFVAR LF@tmp_string2\n");
                        printf("MOVE LF@tmp_string2 string@%s\n", item->token.attribute.string_ptr);
                        printf("DEFVAR LF@tmp_string3\n");
                        printf("CONCAT LF@tmp_string3 LF@tmp_string1 LF@tmp_string2\n");
                        printf("PUSHS LF@tmp_string3\n");    
                    } 
                }
            }
            default:
            {
                addError(OTHER_ERROR, NULL);
            }
        }
    }
}