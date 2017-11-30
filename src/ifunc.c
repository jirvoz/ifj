/*  Course:  Formal Languages and Compilers (IFJ) - FIT VUT Brno 2017/18      */
/*  Project: Implementation of the IFJ17 imperative language translator       */
/*  File:    Source file of lexical analyser                                  */
/*                                                                            */
/*  Authors: Tomáš Nereča : xnerec00 : ()% (team leader)                      */
/*           Samuel Obuch : xobuch00 : ()%                                    */
/*           Jiří Vozár   : xvozar04 : ()%                                    */
/*           Ján Farský   : xfarsk00 : ()%                                    */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include "errors.h"
#include "parser.h"
#include "scanner.h"
#include "symtable.h"
#include "expressions.h"
#include "ifunc.h"

bool callAsc()
{//function gets str and postion in it and returns int related to chars position
 // in ASCII table

    // last_token.type is left bracket

    for (int i = 0; i < 2; i++)
    {
        UPDATE_LAST_TOKEN();

        if (last_token.type == RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function Asc.");

        if (i == 0)
        {
            if(!expression(STRING))
                return false;
        }
        else
        {
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

    printf("STRLEN GF@$num3 GF@$str1\n");

    printf("GT GF@$bool1 GF@$num2 GF@$num3\n");

    printf("JUMPIFEQ &asc_end%d GF@$bool1 bool@true\n", name);

    printf("LT GF@$bool1 GF@$num2 int@1\n");
    printf("JUMPIFEQ &asc_end%d GF@$bool1 bool@true\n", name);
    
    printf("SUB GF@$num2 GF@$num2 int@1\n");
    printf("STRI2INT GF@$num1 GF@$str1 GF@$num2\n");

    printf("LABEL &asc_end%d\n", name);
    printf("PUSHS GF@$num1\n");

    return true;
}

bool callChr()
{//function gets int and returns chr from ASCII table

    // last_token.type is left bracket

    UPDATE_LAST_TOKEN();

    if (last_token.type == RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function Chr.");

    if(!expression(INTEGER))
       return false;

   // Check for colon between parameters
    if (last_token.type == COLON_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Unexpected colon, this function has 1 parameter");

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
       ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    printf("INT2CHARS\n");

    return true;
}

bool callLength()
{//function gets string and returns its length like integer

    // last_token.type is left bracket
    
    UPDATE_LAST_TOKEN();

    if (last_token.type == RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function Length.");

    if(!expression(STRING))
       return false;

    // Check for colon between parameters
    if (last_token.type == COLON_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Unexpected colon, this function has 1 parameter");

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
       ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    printf("POPS GF@$str2\n");
    printf("STRLEN GF@$num1 GF@$str2\n");
    printf("PUSHS GF@$num1\n");

    return true;
}

bool callSubstr()
{//function gets string and returns its length like integer

    // last_token.type is left bracket

    for (int i = 0; i < 3; i++)
    {
        UPDATE_LAST_TOKEN();

        if (last_token.type == RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected more parameters for function SubStr.");

        if (i == 0)
        {
            if(!expression(STRING))
                return false;
        }
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

    //num1 is length sub str
    printf("POPS GF@$num1\n");
    //num2 is start pos
    printf("POPS GF@$num2\n");
    printf("SUB GF@$num2 GF@$num2 int@1\n");
    //str2 is input str 
    printf("POPS GF@$str2\n");
    //cycle counter 
    printf("MOVE GF@$counter int@0\n");
    // str1 is output string
    printf("MOVE GF@$str1 string@\n");

    //if empty string or i <=0
    //num3 is sub_tmp_len
    printf("STRLEN GF@$num3 GF@$str2\n");
    printf("JUMPIFEQ &sub_end%d GF@$num3 int@0\n", name);

    printf("GT GF@$bool1 GF@$num2 int@-1\n");
    printf("JUMPIFNEQ &sub_end%d GF@$bool1 bool@true\n", name);
    
    //if n < 0
    printf("LT GF@$bool1 GF@$num1 int@0\n");
    printf("JUMPIFEQ &cycle2%d GF@$bool1 bool@true\n", name);

    //if n > rest of num1(string)
    printf("SUB GF@$num3 GF@$num3 GF@$num2\n");
    printf("GT GF@$bool1 GF@$num1 GF@$num3\n");
    printf("JUMPIFEQ &cycle2%d GF@$bool1 bool@true\n", name);


    ////////////////////cycle1
    printf("LABEL &cycle1%d\n", name);
    printf("GETCHAR GF@$char GF@$str2 GF@$num2\n");
    //conect pieces of str1urn value
    printf("CONCAT GF@$str1 GF@$str1 GF@$char\n");
    printf("ADD GF@$counter GF@$counter int@1\n");
    printf("ADD GF@$num2 GF@$num2 int@1\n");

    //if counter == num1 i jump to end label
    printf("JUMPIFNEQ &cycle1%d GF@$counter GF@$num1\n", name);
    printf("JUMP &sub_end%d\n", name);

    printf("LABEL &cycle2%d\n", name);
    printf("MOVE GF@$num1 GF@$num3\n");
    printf("JUMP &cycle1%d\n", name);
    
    printf("LABEL &sub_end%d\n", name);
    printf("PUSHS GF@$str1\n");

    return true;
}
