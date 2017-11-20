#include <stdio.h>
#include <malloc.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "stack.h"

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
            printf("INT2FLOAT\n");
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
            if (term.token.type == STRING_TOK)
            {
                if (string_flag)
                    printf("MOVE LF@$tmp_string2 string@%s\n", term.token.attribute.string_ptr);
                else
                {
                    printf("MOVE LF@$tmp_string1 string@%s\n", term.token.attribute.string_ptr);
                    string_flag = true;
                } 
            }
            else if (term.token.type == IDENTIFIER_TOK)
            {
                if (string_flag)
                    printf("MOVE LF@$tmp_string2 string@%s\n", term.token.attribute.string_ptr);
                else
                {
                    printf("MOVE LF@$tmp_string1 GF@%s\n", term.token.attribute.string_ptr);
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
        //'<>' - compare and negate
        case NOT_EQ_IN:
        {
            printf("EQS\n");
            printf("NOTS\n");
        }
            break;
        default:
        {
            addError(OTHER_ERROR, NULL);
        }
    }
}
/*
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