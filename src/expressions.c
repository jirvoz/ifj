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

//this flag signalize if strings were created on the stack
bool string_added;

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

bool getTerm(tTerm* term)
{
    //set token in term
    term->token = last_token;

    //token is identifier, figure out if var or func
    if (last_token.type == IDENTIFIER_TOK)
    {
        //search in func_table
        tSymbol* symbol = htSearch(func_table, last_token.attribute.string_ptr);

        if (symbol != NULL)
        {
            if (symbol->defined)
            {
                //token type 2 == p_table term.index 13 etc.
                if (symbol->type >= 2 && symbol->type <= 4)
                {
                    term->index = symbol->type + 11;
                    return true;
                }
                addError(SEM_TYPE_ERROR, "Bad return type of function");
                return false;
            }
            addError(SEM_PROG_ERROR, "Undefined function");
            return false;
        }
        //search in var_table
        symbol = htSearch(var_table, last_token.attribute.string_ptr);

        if (symbol != NULL)
        {
            if (symbol->type >= 2 && symbol->type <= 4)
            {
                //token type 2 == p_table term.index 13 etc.
                term->index = symbol->type + 11;
                return true;
            }
            addError(SEM_TYPE_ERROR, "Unknown variable type");
            return false;
        }
    }
    //int, float and string constants
    else if (last_token.type > 1 && last_token.type < 5)
    {
        term->index = last_token.type + 11;
    }
    //operators
    else if(last_token.type >= 10 && last_token.type <= 22)
    {
        term->index = last_token.type - 10;
    }
    //if other token_type, term.index is DOLAR - end of expression
    else
    {
        term->index = DOLAR_IN;
    }
    return true;
}

//main expression function
bool expression(token_type expected_type)
{
    UPDATE_LAST_TOKEN();

    token_type return_type = expected_type;

    if (expected_type == UNDEFINED_TOK || expected_type == BOOLEAN ) // Undefined token set by first token type
        return_type = last_token.type;

    switch (return_type)
    {
        case INTEGER_TOK:
        case FLOATING_POINT_TOK:
            return (postNumber(expected_type, return_type));
            break;
        case STRING_TOK:
            return (postString(expected_type, return_type));
            break;
        default:
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown token type");
    }
}

bool postNumber(token_type expected_type, token_type return_type)
{
    tTerm term;
    tTerm* stack_term;

    tStack* stack = stackInit();

    int operand_count = 0;
    int operation_count = 0;
    int parentals_count = 0;
    bool logic_allowed = true;

    while (getTerm(&term))
    {
        if (term.index == INT_IN || term.index == DOUBLE_IN)
        {
            operand_count++;
            generateInstruction(return_type, term);
            UPDATE_LAST_TOKEN();
        }
        else if (term.index == PLUS_IN || term.index == MINUS_IN || term.index == MUL_IN || term.index == FLOAT_DIV_IN || term.index == INT_DIV_IN)  //Operands and operations allowed when expected type is INTEGER or DOUBLE      
        {
            operation_count++;

            if (stackEmpty(stack))
            {
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
            else
            {   
                int get_priority = '>'; //first state for start cycle

                while (get_priority != '<')
                {
                    if (!stackEmpty(stack))
                    {
                        stack_term = stackTop(stack);
                        get_priority = precedence_table[stack_term->index][term.index];
                    }
    
                    if (get_priority == '<' || stackEmpty(stack))
                    {
                        stackPush(stack, term);
                        UPDATE_LAST_TOKEN();
                        break;
                    }
                    else
                    {
                        stack_term = stackPop(stack);
                        generateInstruction(return_type, *stack_term);
                    }
                }
            }
        }
        else if (term.index == LEFT_PARENT_IN || term.index == RIGHT_PARENT_IN)
        {
            if (term.index == LEFT_PARENT_IN)
            {
                parentals_count++;
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
            else
            {   
                if (parentals_count > 0)
                {
                    parentals_count--;
                    stack_term = stackPop(stack);

                    while (stack_term->index != LEFT_PARENT_IN) 
                    {
                        generateInstruction(return_type, *stack_term);
                        stack_term = stackPop(stack);                        
                    }
                    UPDATE_LAST_TOKEN();
                }
                else
                {
                    free(stack);
                    ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad number of parentals in expression");
                }     
            }
        }
        else if((term.index == EQ_EXPR_IN || term.index == NOT_EQ_IN || term.index == LESS_EQ_IN ||
                 term.index == MORE_EQ_IN || term.index == LESS_IN || term.index == MORE_IN) && 
                (expected_type == BOOLEAN || expected_type == UNDEFINED_TOK))
        {
            if (logic_allowed) 
            {
                return_type = BOOLEAN;
                logic_allowed = false;
    
                operation_count++;
    
                if (stackEmpty(stack))
                {
                    stackPush(stack, term);
                    UPDATE_LAST_TOKEN();
                }
                else
                {   
                    int get_priority = '>'; //first state for start cycle
    
                    while (get_priority != '<')
                    {
                        if (!stackEmpty(stack))
                        {
                            stack_term = stackTop(stack);
                            get_priority = precedence_table[stack_term->index][term.index];
                        }

                        if (get_priority == '<' || stackEmpty(stack))
                        {
                            stackPush(stack, term);
                            UPDATE_LAST_TOKEN();
                            break;
                        }
                        else
                        {
                            stack_term = stackPop(stack);
                            generateInstruction(return_type, *stack_term);
                        }
                    }
                }
            }
            else
            {
                free(stack);
                ERROR_AND_RETURN(SEM_TYPE_ERROR,"More than one relation operation in expression");
            }
        }
        else if (term.index == DOLAR_IN)
        {
            if ((parentals_count == 0) && ((operation_count + 1) == operand_count)) //
            {
                while (!stackEmpty(stack))
                {
                    stack_term = stackPop(stack);
                    generateInstruction(return_type, *stack_term); 
                }
                free(stack);
                return true;
            }
            else
            {
                free(stack);
                ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad number of operations or operands in expression");  
            }
        }
        else
        {
            free(stack);
            ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad operation or operand in expression");
        }
    }

    return false;
}

bool postString(token_type expected_type, token_type return_type)
{
    string_added = false;


    tTerm term;
    tTerm* stack_term;

    tStack* stack = stackInit();

    int string_count = 0;
    int operation_count = 0;
    int parentals_count = 0;
    bool logic_allowed = true;

    while (getTerm(&term))
    {
        if (term.index == STRING_IN)
        {
            string_count++;
            generateInstruction(return_type, term);
            UPDATE_LAST_TOKEN();
        }
        else if (term.index == PLUS_IN)
        {
            operation_count++;

            if (stackEmpty(stack))
            {
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
            else
            {
                stack_term = stackPop(stack);
                generateInstruction(return_type, *stack_term);
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
        }
        else if (term.index == LEFT_PARENT_IN || term.index == RIGHT_PARENT_IN)
        {
            if (term.index == LEFT_PARENT_IN)
            {
                parentals_count++;
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
            else
            {   
                if (parentals_count > 0)
                {
                    parentals_count--;
                    stack_term = stackPop(stack);

                    while (stack_term->index != LEFT_PARENT_IN) 
                    {
                        generateInstruction(return_type, *stack_term);
                        stack_term = stackPop(stack);                        
                    }
                    UPDATE_LAST_TOKEN();
                }
                else
                {
                    free(stack);
                    ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad number of parentals in expression");
                }     
            }
        }
        else if((term.index == EQ_EXPR_IN || term.index == NOT_EQ_IN || term.index == LESS_EQ_IN ||
                 term.index == MORE_EQ_IN || term.index == LESS_IN || term.index == MORE_IN) && 
                (expected_type == BOOLEAN || expected_type == UNDEFINED_TOK))
        {
            if (logic_allowed) 
            {
                return_type = BOOLEAN;
                logic_allowed = false;
    
                operation_count++;
    
                if (stackEmpty(stack))
                {
                    stackPush(stack, term);
                    UPDATE_LAST_TOKEN();
                }
                else
                {   
                    int get_priority = '>'; //first state for start cycle
    
                    while (get_priority != '<')
                    {
                        if (!stackEmpty(stack))
                        {
                            stack_term = stackTop(stack);
                            get_priority = precedence_table[stack_term->index][term.index];
                        }

                        if (get_priority == '<' || stackEmpty(stack))
                        {
                            stackPush(stack, term);
                            UPDATE_LAST_TOKEN();
                            break;
                        }
                        else
                        {
                            stack_term = stackPop(stack);
                            generateInstruction(return_type, *stack_term);
                        }
                    }
                }
            }
            else
            {
                free(stack);
                ERROR_AND_RETURN(SEM_TYPE_ERROR,"More than one relation operation in expression");
            }
        }
        else if (term.index == DOLAR_IN)
        {
            if ((parentals_count == 0) && ((operation_count + 1) == string_count)) //
            {
                while (!stackEmpty(stack))
                {
                    stack_term = stackPop(stack);
                    generateInstruction(return_type, *stack_term); 
                }
                free(stack);
                return true;
            }
            else
            {
                free(stack);
                ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad number of operations or strings in expression");  
            }
        }
        else
        {
            free(stack);
            ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad operation or operand in expression");
        }
    }

    return false;
}

bool generateInstruction(token_type return_type, tTerm term)
{
    //just for testing
    //printTerm(term);

    //prepare string variables in Local Frame
    //term.term.index - expected type is bool but, that strings will be compared
    if ((string_added == false) && (term.index == STRING_IN))
    {
        printf("DEFVAR LF@$tmp_string1\n");
        printf("DEFVAR LF@$tmp_string2\n");
        printf("DEFVAR LF@$tmp_string3\n");
    }

    //function calling, value after calling will be on the top of stack
    if (term.token.type == IDENTIFIER_TOK)
    {
        tSymbol* symbol = htSearch(func_table, term.token.attribute.string_ptr);
        if (symbol != NULL)
        {
            call(term.token.attribute.string_ptr);

            //if return type is INT, convert to double
            if (symbol->type == INTEGER_TOK)
            {
                printf("INT2FLOATS\n");
            }
        }
    }

    //main switch
    switch (term.index)
    {
        //push integer to stack
        case INT_IN:
        {
            //constant
            if (term.token.type == INTEGER_TOK)
                printf("PUSHS int@%d\n", term.token.attribute.number);
            //identifier
            else
                printf("PUSHS LF@%s\n", term.token.attribute.string_ptr);
            //convert to float
            printf("INT2FLOATS\n");
        }
            break;
        //push float to stack
        case DOUBLE_IN:
        {
            //constant
            if (term.token.type == FLOATING_POINT_TOK)
                printf("PUSHS float@%g\n", term.token.attribute.float_number);
            //identifier
            else
                printf("PUSHS LF@%s\n", term.token.attribute.string_ptr);
        }
            break;
        //ADDS instruction or STRING CONCANTENATION
        case PLUS_IN:
        {
            //if int or float, add
            if (!string_added)
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
            if (term.token.type == STRING_TOK)
            {
                if (string_added)
                    printf("MOVE LF@$tmp_string2 string@%s\n", term.token.attribute.string_ptr);
                else
                {
                    printf("MOVE LF@$tmp_string1 string@%s\n", term.token.attribute.string_ptr);
                    string_added = true;
                } 
            }
            else if (term.token.type == IDENTIFIER_TOK)
            {
                if (string_added)
                    printf("MOVE LF@$tmp_string2 string@%s\n", term.token.attribute.string_ptr);
                else
                {
                    printf("MOVE LF@$tmp_string1 GF@%s\n", term.token.attribute.string_ptr);
                    string_added = true;
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
        //'<>' - compare and negate
        case NOT_EQ_IN:
        {
            printf("EQS\n");
            printf("NOTS\n");
        }
            break;
        case DOLAR_IN:
        {
            if (return_type == INTEGER_TOK)
            {
                printf("FLOAT2R2EINTS LF\n");
            }
        }
            break;
        default:
        {
            ERROR_AND_RETURN(SEM_TYPE_ERROR, "Wrong expression");
        }
    }
    return true;
}
