//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        expressions.c
//  Description: Source file of expressions module
//               Parser calls expression for evaluation, result from expression
//               after calling is on the top of stack
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include <stdio.h>
#include <stdlib.h>
#include "expressions.h"
#include "errors.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "ifunc.h"

// Size of precedence table
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

// Main expression function
bool expression(token_type expected_type)
{
    tTerm term;

    // Last_token is first token of expression
    if (!getTerm(&term) || term.index == DOLAR_IN)
        ERROR_AND_RETURN(SYN_ERROR, "Empty expression");

    tStack* stack = stackInit();

    token_type return_type = expected_type;		// Return_type is token type for internal comunication whether we are working with a number or string

    if (expected_type == UNDEFINED_TOK || expected_type == BOOLEAN ) // Undefined token set by first token type
    {
        while (term.index == LEFT_PARENTH_IN)    // Firstly left brackets will be pushed on stack
        {
            stackPush(stack, term);
            UPDATE_LAST_TOKEN();
            getTerm(&term);
        }

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
                stackFree(stack);
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
            stackFree(stack);
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown token type");
    }
}

// This function set the type of token and index in precedence table
bool getTerm(tTerm* term)
{
    // Set token in term
    term->token = last_token;

    switch(last_token.type)
    {   
        // Token is identifier, figure out if var or func
        case IDENTIFIER_TOK:
        {
            // Search in func_table
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
                // After function name has to be '('
                if (last_token.type != LEFT_PARENTH_OP)
                    ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");

                return true; 
            }

            // Search in var_table
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
            // If last term was operator, next has to be operand 
            if (term->index >= INT_IN && term->index <= STRING_IN)
                ERROR_AND_RETURN(SYN_ERROR, "Unexpected operand in expression"); 

            UPDATE_LAST_TOKEN();
            // Someting undeclared, figure out what
            if (last_token.type == LEFT_PARENTH_OP)
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Undeclared function");
            else
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Undeclared variable");
        }
            break;
        // Inbuild functions calling
        case ASC:
        {
            term->index = INT_IN;
            term->token.type = ASC;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
        }
            break;
        case CHR:
        {
            term->index = STRING_IN;
            term->token.type = CHR;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
        }
            break;
        case LENGTH:
        {
            term->index = INT_IN;
            term->token.type = LENGTH;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Expected '(' after function");
        }
            break;
        case SUBSTR:
        {
            term->index = STRING_IN;
            term->token.type = SUBSTR;
            UPDATE_LAST_TOKEN();
            if (last_token.type != LEFT_PARENTH_OP)
                ERROR_AND_RETURN(SEM_TYPE_ERROR, "Expected '(' after function");
        }
            break;
        default:
        {   
            // Int, float and string constants
            if (last_token.type >= INTEGER_TOK && last_token.type <= STRING_TOK)
            {
                term->index = last_token.type + 12;
            }
            // Operators
            else if (last_token.type >= EQUAL_SIGN_OP && last_token.type <= RIGHT_PARENTH_OP)
            {
                term->index = last_token.type - 10;
            }
            // If other token_type, term->index is DOLAR - end of expression
            else
                term->index = DOLAR_IN;
        }
    }
    return true;
}

// Function for check operation priority in the precedence table
bool getPriority (tTerm term, tStack* stack, token_type return_type)
{
    int get_priority = '>'; // First state for start cycle
    tTerm tmp_term;

    while (get_priority != '<')
    {
        if (!stackEmpty(stack))	// If stack is not empty get sign from stack
        {
            tmp_term = stackTop(stack);
            get_priority = precedence_table[tmp_term.index][term.index];	// Getting priority
        }
            
        if (get_priority == '<' || stackEmpty(stack))	// If priority is lower sign pushed on stack
        {
            stackPush(stack, term);
            break;
        }
        else	// Else pop from stack
        {
            tmp_term = stackPop(stack);
            generateInstruction(return_type, tmp_term);
        }
    }
    return true;
}

// Function for conversion from infix expression to postfix expresion for instruction generator
bool postfix(token_type expected_type, token_type return_type, tTerm term, tStack* stack)
{
    tTerm stack_term;

    int operand_count = 0;			// Variable for counting number of operands
    int operation_count = 0;		// Variable for counting number of operations
    bool logic_allowed = true;		// After first logical equality sign set to false
    bool negative_number = false; 	// If befor number comes + or - set tu true	

    do
    {
        if ((((term.index == INT_IN) || (term.index == DOUBLE_IN)) && (return_type != STRING)) ||	// checking allowed types in expresion
            ((term.index == STRING_IN) && (return_type == STRING)))
        {
            if(negative_number && return_type != STRING)        // If firstly in expresion is sign + or -
            {
                if (term.index == INT_IN)
                    return_type = INTEGER;
                else
                    return_type = DOUBLE;

                negative_number = false;
            }

            if ((expected_type == UNDEFINED_TOK) && (term.index == DOUBLE_IN)  && (return_type != STRING)) //if is in undefined expression Double whole expression convert to Double
                return_type = DOUBLE;

            operand_count++;
            generateInstruction(return_type, term);
            UPDATE_LAST_TOKEN();
        }
        else if (((term.index == PLUS_IN || term.index == MINUS_IN || term.index == MUL_IN || term.index == FLOAT_DIV_IN || term.index == INT_DIV_IN) && return_type != STRING) ||
                ((term.index == PLUS_IN) && (return_type == STRING)))  // Operands and operations allowed when for numerical or string expressions
        {
            if (operand_count == 0)     // Negative_number + or - solved ... firstly push 0.0
            {
                negative_number = true;
                operand_count++;
                tTerm tmp_term;
                tmp_term.index = DOUBLE_IN;
                tmp_term.token.type = DOUBLE_TOK;
                tmp_term.token.attribute.float_number = 0.0;

                generateInstruction(return_type, tmp_term);
            }

            if ((expected_type == UNDEFINED_TOK) && (term.index == FLOAT_DIV_IN))   // If in undefined expression is float div then convert to double
            {
                return_type = DOUBLE;
            }

            operation_count++;

            if (stackEmpty(stack))		// For empty stack sign will be pushed to stack
            {
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
            else
            {   
                if (getPriority (term, stack, return_type))		// If stack is not empty then call function for checking priority
                    UPDATE_LAST_TOKEN();
            }
        }
        else if (term.index == LEFT_PARENTH_IN || term.index == RIGHT_PARENTH_IN)	// If comes parenthesis 
        {
            if (term.index == LEFT_PARENTH_IN)	// Left parenthesis will be pushed on stack
            {
                stackPush(stack, term);
                UPDATE_LAST_TOKEN();
            }
            else
            {
                stack_term = stackTop(stack);	

                while (!(stackEmpty(stack)) && (stack_term.index != LEFT_PARENTH_IN)) // While is stack not empty or left parenthesis on top
                {
                    stack_term = stackPop(stack);
                    generateInstruction(return_type, stack_term);
                    stack_term = stackTop(stack);                    
                }

                if (stackEmpty(stack)) // If comes right parenthesis and stack is empty then it is end of expression
                {
                    tTerm tmp_term; 
                    tmp_term.index = DOLAR_IN;
                    tmp_term.token.type = EOL_TOK; // Token_type musn't be empty
                    generateInstruction(return_type, tmp_term);
                    stackFree(stack);
                    return true;
                }
                else
                {
                    stackPop(stack);
                    UPDATE_LAST_TOKEN();
                }     
            }
        }
        else if((term.index == EQ_IN || term.index == NOT_EQ_IN || term.index == LOWER_EQ_IN ||			// if comes logical operation 
                 term.index == HIGHER_EQ_IN || term.index == LOWER_IN || term.index == HIGHER_IN) && 
                (expected_type == BOOLEAN || expected_type == UNDEFINED_TOK))
        {
            if (logic_allowed) 	// Checking if is it first logical operation in expression
            {
                logic_allowed = false;
    
                operation_count++;
    
                if (stackEmpty(stack))			// Same checking priority as clasic sign operations
                {
                    stackPush(stack, term);
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
                stackFree(stack);
                ERROR_AND_RETURN(SYN_ERROR,"More than one relation operation in expression");
            }
        }
        else if (term.index == DOLAR_IN)		// End of expresion is signed as DOLLAR
        {
            if (((operation_count + 1) == operand_count)) // Condition for number of operations and operands
            {
                while (!stackEmpty(stack))		// If stack is not empty then poping everything
                {
                    stack_term = stackPop(stack);

                    if (stack_term.index == LEFT_PARENTH_IN)		// Left parenthesis without right is error
                    {
                        stackFree(stack);
                        ERROR_AND_RETURN(SYN_ERROR,"Bad number of brackets in expression");
                    }
                    else
                       generateInstruction(return_type, stack_term);  
                }
                if (logic_allowed && expected_type == BOOLEAN)
                {
                    stackFree(stack);
                    ERROR_AND_RETURN(SEM_TYPE_ERROR,"Expected equality operation"); 
                }

                if (expected_type == BOOLEAN && return_type != BOOLEAN)		// Setting return type as boolean
                    return_type = BOOLEAN;

                generateInstruction(return_type, term);
                stackFree(stack);
                return true;
            }
            else
            {
                stackFree(stack);
                ERROR_AND_RETURN(SYN_ERROR,"Bad number of operations or operands in expression");  
            }
        }
        else
        {
            stackFree(stack);
            ERROR_AND_RETURN(SEM_TYPE_ERROR,"Bad operation or operand in expression");
        }
    } while (getTerm(&term));	// Calling function getTerm (checking if is variable defined, is allowed etc.)

    stackFree(stack);
    return false;
}

bool generateInstruction(token_type return_type, tTerm sent_term)
{
    // Inbuild function calling
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


    // User function calling, value after calling will be on the top of stack
    if (sent_term.token.type == IDENTIFIER_TOK)
    {
        tSymbol* symbol = htSearch(func_table, sent_term.token.attribute.string_ptr);
        if (symbol != NULL)
        {
            if (!call(sent_term.token.attribute.string_ptr))
                return false;

            // If return type is INT, convert to double
            if (symbol->type == INTEGER)
                printf("INT2FLOATS\n");
            return true;
        }
    }

    // Main switch
    switch (sent_term.index)
    {
        // Push integer to stack
        case INT_IN:
        {
            // Constant
            if (sent_term.token.type == INTEGER_TOK)
                printf("PUSHS int@%d\n", sent_term.token.attribute.number);
            // Identifier
            else
                printf("PUSHS LF@%s\n", sent_term.token.attribute.string_ptr);
            // Convert to float
            printf("INT2FLOATS\n");
        }
            break;
        // Push float to stack
        case DOUBLE_IN:
        {
            // Constant
            if (sent_term.token.type == DOUBLE_TOK)
                printf("PUSHS float@%g\n", sent_term.token.attribute.float_number);
            // Identifier
            else
                printf("PUSHS LF@%s\n", sent_term.token.attribute.string_ptr);
        }
            break;
        // ADDS instruction or STRING CONCANTENATION
        case PLUS_IN:
        {
            // If int or float, add
            if (return_type != STRING)
                printf("ADDS\n");

            // If string, concatenate
            else
            {
                printf("POPS GF@$str2\n"
                       "POPS GF@$str1\n"
                       "CONCAT GF@$str1 GF@$str1 GF@$str2\n"
                       "PUSHS GF@$str1\n");
            }
        }
            break;
        // SUBS instruction
        case MINUS_IN: printf("SUBS\n");
            break;
        // MULS instruction
        case MUL_IN: printf("MULS\n");
            break;
        // Classic DIVS
        case FLOAT_DIV_IN: printf("DIVS\n");
            break;
        // '\' DIVS - integers
        case INT_DIV_IN:
        {   
            printf("POPS GF@$num1\n"
                   "FLOAT2R2EINTS\n"
                   "INT2FLOATS\n"
                   "PUSHS GF@$num1\n"
                   "FLOAT2R2EINTS\n"
                   "INT2FLOATS\n");

            printf("DIVS\n"
                   "FLOAT2INTS\n"
                   "INT2FLOATS\n");
        }
            break;
        // Push String to stack
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
        // '<'- comparison by LTS instruction - automatically pops flag to stack
        case LOWER_IN: printf("LTS\n");   
            break;
        // '>' - comparison by GTS instruction - automatically pops flag to stack
        case HIGHER_IN:
            printf("GTS\n");
            break;
        // '<=' - it's necessary to use also ORS instructions
        case LOWER_EQ_IN:
        {
            printf("GTS\n");
            printf("NOTS\n");
        }
            break;
        // '>=' - it's necessary to use also ORS instructions
        case HIGHER_EQ_IN:
        {
            printf("LTS\n");
            printf("NOTS\n"); 
        }
            break;
        // '=' - use simple EQS instruction
        case EQ_IN: printf("EQS\n");
            break;
        // '<>' - compare and negate
        case NOT_EQ_IN:
        {
            printf("EQS\n");
            printf("NOTS\n");
        }
            break;
        case DOLAR_IN:
        {
            // If return type is integer, convert
            if (return_type == INTEGER)
                printf("FLOAT2R2EINTS\n");
        }
            break;
        default: ERROR_AND_RETURN(SEM_TYPE_ERROR, "Wrong expression");
    }
    return true;
}
