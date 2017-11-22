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
bool string_added = false;
bool simple_bool = false;

//global variable for expressions
tTerm term;

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

//main expression function
bool expression(token_type expected_type)
{
    // last_token is first token of expression

    if (!getTerm() || term.index == DOLAR_IN)
    {
        return false;
    }

    token_type return_type = expected_type;

    if (expected_type == UNDEFINED_TOK || expected_type == BOOLEAN ) // Undefined token set by first token type
    {
        switch (term.index)
        {
            case INT_IN: return_type = INTEGER;
                break;
            case MINUS_IN:
            case PLUS_IN:
            case DOUBLE_IN: return_type = DOUBLE;
                break;
            case STRING_IN: return_type = STRING;
                break;
            default:
                addError(SEM_TYPE_ERROR, "Wrong expression");
                return false;
        }
    }

    switch (return_type)
    {
        case INTEGER:
        case DOUBLE:
            return (postNumber(expected_type, return_type));
            break;
        case STRING:
            return (postString(expected_type, return_type));
            break;
        default:
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown token type");
    }
}

bool getTerm()
{
    //set token in term
    term.token = last_token;

    //token is identifier, figure out if var or func
    if (last_token.type == IDENTIFIER_TOK)
    {
        //search in func_table
        tSymbol* symbol = htSearch(func_table, last_token.attribute.string_ptr);

        if (symbol != NULL)
        {
            if (symbol->defined)
            {
                switch (symbol->type)
                {
                    case INTEGER: term.index = INT_IN;
                        break;
                    case DOUBLE: term.index = DOUBLE_IN;
                        break;
                    case STRING: term.index = STRING_IN;
                        break;
                    default:
                        ERROR_AND_RETURN(SEM_TYPE_ERROR, "Bad return type of function");
                }
                return true;
            }
            ERROR_AND_RETURN(SEM_PROG_ERROR, "Undefined function");
        }
        //search in var_table
        symbol = htSearch(var_table, last_token.attribute.string_ptr);

        if (symbol != NULL)
        {
           switch (symbol->type)
                {
                    case INTEGER: term.index = INT_IN;
                        break;
                    case DOUBLE: term.index = DOUBLE_IN;
                        break;
                    case STRING: term.index = STRING_IN;
                        break;
                    default:
                        addError(SEM_TYPE_ERROR, "Unknown variable type");
                        return false;
                }
            return true;
        }
        UPDATE_LAST_TOKEN();
        if (last_token.type == LEFT_PARENTH_OP)
            ERROR_AND_RETURN(SEM_PROG_ERROR, "Undeclared function");
        else
            ERROR_AND_RETURN(SEM_PROG_ERROR, "Undeclared variable");   
    }
    //int, float and string constants
    else if (last_token.type >= INTEGER_TOK && last_token.type <= STRING_TOK)
    {
        term.index = last_token.type + 11;
    }
    //operators
    else if (last_token.type >= EQUAL_SIGN_OP && last_token.type <= RIGHT_PARENTH_OP)
    {
        term.index = last_token.type - 10;
    }
    //if other token_type, term.index is DOLAR - end of expression
    else
    {
        term.index = DOLAR_IN;
    }
    return true;
}

bool postNumber(token_type expected_type, token_type return_type)
{
    tTerm* stack_term;

    tStack* stack = stackInit();

    int operand_count = 0;
    int operation_count = 0;
    int parentals_count = 0;
    bool logic_allowed = true;
    bool unary = false;

    while (getTerm())
    {
        if (term.index == INT_IN || term.index == DOUBLE_IN)
        {
            if(unary)
            {
                if (term.index == INT_IN)
                {
                    return_type = INTEGER;
                }
                else
                {
                    return_type = DOUBLE;
                }
                unary = false;
            }
            operand_count++;
            generateInstruction(return_type, term);
            UPDATE_LAST_TOKEN();
        }
        else if (term.index == PLUS_IN || term.index == MINUS_IN || term.index == MUL_IN || term.index == FLOAT_DIV_IN || term.index == INT_DIV_IN)  //Operands and operations allowed when expected type is INTEGER or DOUBLE      
        {
            if (operand_count == 0)     //unary + or - solved ... firstly push 0.0
            {
                unary = true;
                operand_count++;
                tTerm tmp_term;
                tmp_term.index = DOUBLE_IN;
                tmp_term.token.type = FLOATING_POINT_TOK;
                tmp_term.token.float_number = 0.0;

                generateInstruction(return_type, tmp_term);
            }
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
                if (logic_allowed)
                {
                    simple_bool = true; 
                }
                generateInstruction(return_type, term);
                free(stack);
                return true;
            }
            else
            {
                free(stack);
                ERROR_AND_RETURN(SYN_ERROR,"Bad number of operations or operands in expression");  
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
    string_added = true;

    tTerm* stack_term;

    tStack* stack = stackInit();

    int string_count = 0;
    int operation_count = 0;
    int parentals_count = 0;
    bool logic_allowed = true;

    while (getTerm())
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
                generateInstruction(return_type, term); 
                free(stack);
                return true;
            }
            else
            {
                free(stack);
                ERROR_AND_RETURN(SYN_ERROR,"Bad number of operations or strings in expression");  
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

bool generateInstruction(token_type return_type, tTerm sent_term)
{
    //just for testing
    //printTerm(sent_term);

    //function calling, value after calling will be on the top of stack
    if (sent_term.token.type == IDENTIFIER_TOK)
    {
        tSymbol* symbol = htSearch(func_table, sent_term.token.attribute.string_ptr);
        if (symbol != NULL)
        {
            UPDATE_LAST_TOKEN();
            if (!call(sent_term.token.attribute.string_ptr))
                return false;

            //if return type is INT, convert to double
            if (symbol->type == INTEGER)
            {
                printf("INT2FLOATS\n");
            }
        }
    }

    //main switch
    switch (sent_term.index)
    {
        //push integer to stack
        case INT_IN:
        {
            //constant
            if (sent_term.token.type == INTEGER_TOK)
                printf("PUSHS int@%d\n", sent_term.token.attribute.number);
            //identifier
            else
                printf("PUSHS LF@%s\n", sent_term.token.attribute.string_ptr);
            //convert to float
            printf("INT2FLOATS\n");
        }
            break;
        //push float to stack
        case DOUBLE_IN:
        {
            //constant
            if (sent_term.token.type == FLOATING_POINT_TOK)
                printf("PUSHS float@%g\n", sent_term.token.attribute.float_number);
            //identifier
            else
                printf("PUSHS LF@%s\n", sent_term.token.attribute.string_ptr);
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
                printf("CREATEFRAME\n");
                printf("DEFVAR TF@tmp1\n");
                printf("DEFVAR TF@tmp2\n");
                printf("DEFVAR TF@tmp3\n");
                printf("POPS TF@tmp2\n");
                printf("POPS TF@tmp1\n");
                printf("CONCAT TF@tmp3 TF@tmp1 TF@tmp2\n");
                printf("PUSHS TF@tmp3\n");
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
            if (sent_term.token.type == STRING_TOK)
                printf("PUSHS string@%s\n", sent_term.token.attribute.string_ptr);
            else
                printf("PUSHS LF@%s\n", sent_term.token.attribute.string_ptr);
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
            if (return_type == INTEGER)
            {
                printf("FLOAT2R2EINTS\n");
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
