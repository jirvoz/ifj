#include <stdio.h>
#include <malloc.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "ifunc.h"

//size of precedence table
#define P_TAB_SIZE 14

const int precedence_table[P_TAB_SIZE][P_TAB_SIZE] =
{
//'=' '<>' '<=' '>='  '<'  '>'  '+'  '-'  '*'  '/'  '\'  '('  ')'  '$'
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'}, //'='
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'}, //'<>'
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'}, //'<='
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'}, //'>='
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'}, //'<'
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'}, //'>'
 {'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>'}, //'+'
 {'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '>'}, //'-'
 {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '>'}, //'*'
 {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '>'}, //'/'
 {'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '>', '<', '>', '>'}, //'\'
 {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', 'x'}, //'('
 {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'x', '>', '>'}, //')'
 {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'x', 'x'}, //'$'
};

//main expression function
bool expression(token_type expected_type)
{
    tTerm* term = malloc(sizeof(tTerm));
    if (term == NULL)
    {
        memoryClear(term, NULL);
        ERROR_AND_RETURN(OTHER_ERROR, "Memmory allocation error");
    }

    // last_token is first token of expression
    if (!getTerm(term) || term->index == DOLAR_IN)
    {
        memoryClear(term, NULL);
        ERROR_AND_RETURN(SYN_ERROR, "Empty expression");
    }
    tStack* stack = stackInit();

    token_type return_type = expected_type;

    if (expected_type == UNDEFINED_TOK || expected_type == BOOLEAN ) // Undefined token set by first token type
    {
        while (term->index == LEFT_PARENTH_IN)    //first left brackets push on stack
        {
            stackPush(stack, *term);
            UPDATE_LAST_TOKEN();
            getTerm(term);
        }

        switch (term->index)
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
                memoryClear(term, stack);
                ERROR_AND_RETURN(SYN_ERROR, "Wrong expression");
        }
    }

    switch (return_type)
    {
        case INTEGER:
        case DOUBLE:
        case STRING:
            return (postfix(expected_type, return_type, term, stack));
            break;
        default:
            memoryClear(term, stack);
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown token type");
    }
}

bool getTerm(tTerm* term)
{
    //set token in term
    term->token = last_token;

    switch(last_token.type)
    {   //token is identifier, figure out if var or func
        case IDENTIFIER_TOK:
        {
            //search in func_table
            tSymbol* symbol = htSearch(func_table, last_token.attribute.string_ptr);

            if (symbol != NULL)
            {
                switch (symbol->type)
                {
                    case INTEGER: term->index = INT_IN;
                        break;
                    case DOUBLE: term->index = DOUBLE_IN;
                        break;
                    case STRING: term->index = STRING_IN;
                        break;
                    default:
                        ERROR_AND_RETURN(SEM_PROG_ERROR, "Bad return type of function");
                }
                UPDATE_LAST_TOKEN();
                if (last_token.type != LEFT_PARENTH_OP)
                {
                    ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
                }
                return true; 
            }

            //search in var_table
            symbol = htSearch(var_table, last_token.attribute.string_ptr);

            if (symbol != NULL)
            {
               switch (symbol->type)
                    {
                        case INTEGER: term->index = INT_IN;
                            break;
                        case DOUBLE: term->index = DOUBLE_IN;
                            break;
                        case STRING: term->index = STRING_IN;
                            break;
                        default:
                            ERROR_AND_RETURN(SEM_TYPE_ERROR, "Unknown variable type");
                    }
                return true;
            }

            if (term->index >= INT_IN && term->index <= STRING_IN)
            {
                ERROR_AND_RETURN(SYN_ERROR, "Unexpected operand in expression"); 
            }

            UPDATE_LAST_TOKEN();
            if (last_token.type == LEFT_PARENTH_OP)
            {
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Undeclared function");
            }
            else
            {
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Undeclared variable");
            } 
        }
            break;
        //inbuild functions
        case ASC:
        {
            term->index = INT_IN;
            term->token.type = ASC;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
            {
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
            }
        }
            break;
        case CHR:
        {
            term->index = STRING_IN;
            term->token.type = CHR;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
            {
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
            }
        }
            break;
        case LENGTH:
        {
            term->index = INT_IN;
            term->token.type = LENGTH;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
            {
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
            }
        }
            break;
        case SUBSTR:
        {
            term->index = STRING_IN;
            term->token.type = SUBSTR;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
            {
                ERROR_AND_RETURN(SEM_TYPE_ERROR, "Expected '(' after function");
            }
        }
            break;
        default:
        {   //int, float and string constants
            if (last_token.type >= INTEGER_TOK && last_token.type <= STRING_TOK)
            {
                term->index = last_token.type + 12;
            }
            //operators
            else if (last_token.type >= EQUAL_SIGN_OP && last_token.type <= RIGHT_PARENTH_OP)
            {
                term->index = last_token.type - 10;
            }
            //if other token_type, term->index is DOLAR - end of expression
            else
            {
                term->index = DOLAR_IN;
            } 
        }
    }
    return true;
}

bool getPriority (tTerm* term, tStack* stack, token_type return_type)
{
    int get_priority = '>'; //first state for start cycle
    tTerm* tmp_term;

    while (get_priority != '<')
    {
        if (!stackEmpty(stack))
        {
            tmp_term = stackTop(stack);
            get_priority = precedence_table[tmp_term->index][term->index];
        }
            
        if (get_priority == '<' || stackEmpty(stack))
        {
            stackPush(stack, *term);
            break;
        }
        else
        {
            tmp_term = stackPop(stack);
            generateInstruction(return_type, *tmp_term);
        }
    }
    return true;
}

bool postfix(token_type expected_type, token_type return_type, tTerm* term, tStack* stack)
{
    tTerm* stack_term;

    int operand_count = 0;
    int operation_count = 0;
    bool logic_allowed = true;
    bool negative_number = false;

    do
    {
        if ((((term->index == INT_IN) || (term->index == DOUBLE_IN)) && (return_type != STRING)) ||
            ((term->index == STRING_IN) && (return_type == STRING)))
        {
            if(negative_number && return_type != STRING)        //if first in expresion is sign + or -
            {
                if (term->index == INT_IN)
                {
                    return_type = INTEGER;
                }
                else
                {
                    return_type = DOUBLE;
                }
                negative_number = false;
            }

            if ((expected_type == UNDEFINED_TOK) && (term->index == DOUBLE_IN)) //if is in undefined expression Double whole expression convert to Double
            {
                return_type = DOUBLE;
            }

            operand_count++;
            generateInstruction(return_type, *term);
            UPDATE_LAST_TOKEN();
        }
        else if (((term->index == PLUS_IN || term->index == MINUS_IN || term->index == MUL_IN || term->index == FLOAT_DIV_IN || term->index == INT_DIV_IN) && return_type != STRING) ||
                ((term->index == PLUS_IN) && (return_type == STRING)))  //Operands and operations allowed when expected type is INTEGER or DOUBLE      
        {
            if (operand_count == 0)     //negative_number + or - solved ... firstly push 0.0
            {
                negative_number = true;
                operand_count++;
                tTerm tmp_term;
                tmp_term.index = DOUBLE_IN;
                tmp_term.token.type = DOUBLE_TOK;
                tmp_term.token.attribute.float_number = 0.0;

                generateInstruction(return_type, tmp_term);
            }

            if ((expected_type == UNDEFINED_TOK) && (term->index == FLOAT_DIV_IN))   //if in undefined expression is float div then convert to double
            {
                return_type = DOUBLE;
            }

            operation_count++;

            if (stackEmpty(stack))
            {
                stackPush(stack, *term);
                UPDATE_LAST_TOKEN();
            }
            else
            {   
                if (getPriority (term, stack, return_type))
                    UPDATE_LAST_TOKEN();
            }
        }
        else if (term->index == LEFT_PARENTH_IN || term->index == RIGHT_PARENTH_IN)
        {
            if (term->index == LEFT_PARENTH_IN)
            {
                stackPush(stack, *term);
                UPDATE_LAST_TOKEN();
            }
            else
            {
                if (stackEmpty(stack))
                {   
                    tTerm tmp_term; 
                    tmp_term.index = DOLAR_IN;
                    tmp_term.token.type = EOL_TOK; //token_type musn't be empty
                    generateInstruction(return_type, tmp_term);
                    memoryClear(term, stack);
                    return true;
                }

                stack_term = stackTop(stack);

                while (!(stackEmpty(stack)) && (stack_term->index != LEFT_PARENTH_IN)) // in strings weird
                {
                    stack_term = stackPop(stack);  
                    generateInstruction(return_type, *stack_term);
                    stack_term = stackTop(stack);                        
                }

                if (stackEmpty(stack))
                {
                    tTerm tmp_term; 
                    tmp_term.index = DOLAR_IN;
                    tmp_term.token.type = EOL_TOK; //token_type musn't be empty
                    generateInstruction(return_type, tmp_term);
                    memoryClear(term, stack);
                    return true;
                }
                else
                {
                    stackPop(stack);
                    UPDATE_LAST_TOKEN();
                }     
            }
        }
        else if((term->index == EQ_IN || term->index == NOT_EQ_IN || term->index == LOWER_EQ_IN ||
                 term->index == HIGHER_EQ_IN || term->index == LOWER_IN || term->index == HIGHER_IN) && 
                (expected_type == BOOLEAN || expected_type == UNDEFINED_TOK))
        {
            if (logic_allowed) 
            {
                //return_type = BOOLEAN;
                logic_allowed = false;
    
                operation_count++;
    
                if (stackEmpty(stack))
                {
                    stackPush(stack, *term);
                    UPDATE_LAST_TOKEN();
                }
                else
                {   
                    if (getPriority (term, stack, return_type))
                        UPDATE_LAST_TOKEN();
                }
            }
            else
            {
                memoryClear(term, stack);
                ERROR_AND_RETURN(SEM_TYPE_ERROR,"More than one relation operation in expression");
            }
        }
        else if (term->index == DOLAR_IN)
        {
            if (((operation_count + 1) == operand_count)) //
            {
                while (!stackEmpty(stack))
                {
                    stack_term = stackPop(stack);

                    if (stack_term->index == LEFT_PARENTH_IN)
                    {
                        memoryClear(term, stack);
                        ERROR_AND_RETURN(SYN_ERROR,"Bad number of brackets in expression");
                    }
                    else
                    {
                       generateInstruction(return_type, *stack_term);  
                    }
                }
                if (logic_allowed && expected_type == BOOLEAN)
                {
                    memoryClear(term, stack);
                    ERROR_AND_RETURN(SEM_TYPE_ERROR,"Expected equality operation"); 
                }
                if (expected_type == BOOLEAN && return_type != BOOLEAN)
                {
                    return_type = BOOLEAN;
                }
                generateInstruction(return_type, *term);
                memoryClear(term, stack);
                return true;
            }
            else
            {
                memoryClear(term, stack);
                ERROR_AND_RETURN(SYN_ERROR,"Bad number of operations or operands in expression");  
            }
        }
        else
        {
            memoryClear(term, stack);;
            ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad operation or operand in expression");
        }
    } while (getTerm(term));

    memoryClear(term, stack);
    return false;
}

bool generateInstruction(token_type return_type, tTerm sent_term)
{
    //just for testing
    //printTerm(sent_term);

    switch(sent_term.token.type)
    {
        case ASC: 
        {
            if (!callAsc())
                return false;
            printf("INT2FLOATS\n");
            return true;
        }
        case CHR: return callChr() ? true : false;
        case LENGTH: 
        {
            if (!callLength())
                return false;
            printf("INT2FLOATS\n");
            return true;
        }
        case SUBSTR: return callSubstr() ? true : false;
        default:
            break;
    }


    //function calling, value after calling will be on the top of stack
    if (sent_term.token.type == IDENTIFIER_TOK)
    {
        tSymbol* symbol = htSearch(func_table, sent_term.token.attribute.string_ptr);
        if (symbol != NULL)
        {
            if (!call(sent_term.token.attribute.string_ptr))
                return false;

            //if return type is INT, convert to double
            if (symbol->type == INTEGER)
                printf("INT2FLOATS\n");
            return true;
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
            if (sent_term.token.type == DOUBLE_TOK)
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
            if (return_type != STRING)
                printf("ADDS\n");

            //if string, concatenate
            else
            {
                printf("CREATEFRAME\n");
                printf("DEFVAR TF@$tmp1\n");
                printf("DEFVAR TF@$tmp2\n");
                printf("DEFVAR TF@$tmp3\n");
                printf("POPS TF@$tmp2\n");
                printf("POPS TF@$tmp1\n");
                printf("CONCAT TF@$tmp3 TF@$tmp1 TF@$tmp2\n");
                printf("PUSHS TF@$tmp3\n");
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
            printf("DIVS\n");
        }
            break;
        //'\' DIVS - integers
        case INT_DIV_IN:
        {   
            printf("CREATEFRAME\n");

            printf("FLOAT2R2EINTS\n");
            printf("INT2FLOATS\n");
            printf("DEFVAR TF@$tmp2\n");
            printf("POPS TF@$tmp2\n");

            printf("FLOAT2R2EINTS\n");
            printf("INT2FLOATS\n");
            printf("DEFVAR TF@$tmp1\n");
            printf("POPS TF@$tmp1\n");

            printf("DIV TF@$tmp1 TF@$tmp1 TF@$tmp2\n");
            printf("PUSHS TF@$tmp1\n");

            printf("FLOAT2INTS\n");
            printf("INT2FLOATS\n");
        }
            break;
        //strings
        case STRING_IN:
        {
            if (sent_term.token.type == STRING_TOK)
            {
                printf("PUSHS string@%s\n", sent_term.token.attribute.string_ptr);
            	free(sent_term.token.attribute.string_ptr);
            }
	       else
                printf("PUSHS LF@%s\n", sent_term.token.attribute.string_ptr);
        }
            break;
        //'<'- comparison by LTS instruction - automatically pops flag to stack
        case LOWER_IN:
        {
            printf("LTS\n");   
        }
            break;
        //'>' - comparison by GTS instruction - automatically pops flag to stack
        case HIGHER_IN:
        {
            printf("GTS\n");
        }
            break;
        // '<=' - it's necessary to use also ORS instructions
        case LOWER_EQ_IN:
        {
            printf("GTS\n");
            printf("NOTS\n");
        }
            break;
        //'>=' - it's necessary to use also ORS instructions
        case HIGHER_EQ_IN:
        {
            printf("LTS\n");
            printf("NOTS\n"); 
        }
            break;
        //'=' - use simple EQS instruction
        case EQ_IN:
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

void memoryClear(tTerm* term, tStack* stack)
{
    if (stack != NULL)
    {
        stackFree(stack);
        free(stack);
    }

    if (term != NULL)
    {
        free(term);    
    }  
}
