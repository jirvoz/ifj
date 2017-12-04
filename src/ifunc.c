//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        ifunc.c
//  Description: Source file of inbuild functions module
//               Module generates instructions for inbuild functions of IFJ17
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include <stdio.h>
#include <stdbool.h>
#include "ifunc.h"
#include "errors.h"
#include "parser.h"
#include "expressions.h"

// Convert character in string to number from ascii table
bool callAsc()
{
    // Handle arguments
    for (int i = 0; i < 2; i++)
    {
        UPDATE_LAST_TOKEN();

        if (last_token.type == RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function Asc.");

        if (i == 0)
        {
            // First argument is string
            if(!expression(STRING))
                return false;
        }
        else
        {
            // Second argument is number - position in string
            if(!expression(INTEGER))
                return false;
        }

        // Check for colon between parameters
        if (i == 0 && last_token.type != COLON_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected colon between parameters.");
    }

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    static int name = 0;
    name++;

    // num1 is output ASC
    printf("MOVE GF@$num1 int@0\n");
    // num2 is position
    printf("POPS GF@$num2\n");
    // str1 is input string
    printf("POPS GF@$str1\n");

    // If index is out of range, jump to the end and return 0
    printf("STRLEN GF@$num3 GF@$str1\n");
    printf("GT GF@$bool1 GF@$num2 GF@$num3\n");
    printf("JUMPIFEQ &asc_end%d GF@$bool1 bool@true\n", name);

    // If index is less than 1, jump to the end and return 0
    printf("LT GF@$bool1 GF@$num2 int@1\n");
    printf("JUMPIFEQ &asc_end%d GF@$bool1 bool@true\n", name);

    // Do stuff
    printf("SUB GF@$num2 GF@$num2 int@1\n");
    printf("STRI2INT GF@$num1 GF@$str1 GF@$num2\n");

    // End - push number on the top of stack
    printf("LABEL &asc_end%d\n", name);
    printf("PUSHS GF@$num1\n");

    return true;
}

// Convert number from ascii table to character
bool callChr()
{
    // Handle arguments
    UPDATE_LAST_TOKEN();

    if (last_token.type == RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function Chr.");

    // Only 1 argument - number
    if(!expression(INTEGER))
       return false;

    // Check for colon between parameters
    if (last_token.type == COLON_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Unexpected colon, this function has 1 parameter");

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    // Do stuff
    printf("INT2CHARS\n");

    return true;
}

// Return length of string
bool callLength()
{
    // Handle arguments
    UPDATE_LAST_TOKEN();

    if (last_token.type == RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function Length.");

    // Only 1 argument - string
    if(!expression(STRING))
        return false;

    // Check for colon between parameters
    if (last_token.type == COLON_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Unexpected colon, this function has 1 parameter");

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
       ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    // Do stuff
    printf("POPS GF@$str2\n");
    printf("STRLEN GF@$num1 GF@$str2\n");
    printf("PUSHS GF@$num1\n");

    return true;
}

// Return required substring
bool callSubstr()
{
    // Handle arguments
    for (int i = 0; i < 3; i++)
    {
        UPDATE_LAST_TOKEN();

        if (last_token.type == RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function SubStr.");

        // First argument is string
        if (i == 0)
        {
            if(!expression(STRING))
                return false;
        }
        // Second and third argument are numbers
        else
        {
            if(!expression(INTEGER))
                return false;
        }

        // Check for colon between parameters
        if (i < 2 && last_token.type != COLON_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected colon between parameters.");
    }

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    static int name = 0;
    name++;

    // num1 is length sub str
    printf("POPS GF@$num1\n");
    // num2 is starting posistion
    printf("POPS GF@$num2\n");
    printf("SUB GF@$num2 GF@$num2 int@1\n");
    // str2 is input str 
    printf("POPS GF@$str2\n");
    // Cycle counter 
    printf("MOVE GF@$counter int@0\n");
    // str1 is output string
    printf("MOVE GF@$str1 string@\n");

    // If empty string or starting position <= 0 jump to the end and return empty string
    // num3 is tmp length
    printf("STRLEN GF@$num3 GF@$str2\n");
    printf("JUMPIFEQ &sub_end%d GF@$num3 int@0\n", name);
    printf("GT GF@$bool1 GF@$num2 int@-1\n");
    printf("JUMPIFNEQ &sub_end%d GF@$bool1 bool@true\n", name);
    
    // If length of substring < 0 or length > rest of string after starting position
    // return rest of the string after starting position
    printf("LT GF@$bool1 GF@$num1 int@0\n");
    printf("JUMPIFEQ &rest%d GF@$bool1 bool@true\n", name);
    printf("SUB GF@$num3 GF@$num3 GF@$num2\n");
    printf("GT GF@$bool1 GF@$num1 GF@$num3\n");
    printf("JUMPIFEQ &rest%d GF@$bool1 bool@true\n", name);

    // Cycle
    printf("LABEL &cycle%d\n", name);
    // Add char to the end of output string
    printf("GETCHAR GF@$char GF@$str2 GF@$num2\n");
    printf("CONCAT GF@$str1 GF@$str1 GF@$char\n");
    // Increment counter and index
    printf("ADD GF@$counter GF@$counter int@1\n");
    printf("ADD GF@$num2 GF@$num2 int@1\n");

    // If counter == length, jump to end label
    printf("JUMPIFNEQ &cycle%d GF@$counter GF@$num1\n", name);
    printf("JUMP &sub_end%d\n", name);

    // If functions will return rest of string
    printf("LABEL &rest%d\n", name);
    printf("MOVE GF@$num1 GF@$num3\n");
    printf("JUMP &cycle%d\n", name);
    
    // Push substring on the top of stack
    printf("LABEL &sub_end%d\n", name);
    printf("PUSHS GF@$str1\n");

    return true;
}
