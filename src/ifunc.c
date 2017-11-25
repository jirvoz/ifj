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

     ////////////////////Asc function
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@$ret\n");
    printf("DEFVAR TF@$pos\n");
    printf("POPS TF@$pos\n");
    printf("SUB TF@$pos TF@$pos int@1\n");
    printf("DEFVAR TF@$str1\n");
    printf("POPS TF@$str1\n");
    
    printf("STRI2INT TF@$ret TF@$str1 TF@$pos\n");
    printf("PUSHS TF@$ret\n");

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

    printf("CREATEFRAME\n");
    printf("DEFVAR TF@$ret\n");
    printf("DEFVAR TF@$str1\n");
    printf("POPS TF@$str1\n");
    printf("STRLEN TF@$ret TF@$str1\n");
    printf("PUSHS TF@$ret\n");

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

    static int name = 1;
    name++;



    ////////////////base function
    printf("CREATEFRAME\n");
    //length of sub str
    printf("DEFVAR TF@$length\n");
    printf("POPS TF@$length\n");
    //start pos
    printf("DEFVAR TF@$start_pos\n");
    printf("POPS TF@$start_pos\n");
    printf("SUB TF@$start_pos TF@$start_pos int@1\n");
    //input str
    printf("DEFVAR TF@$in_str\n");
    printf("POPS TF@$in_str\n");
    //int vars
    printf("DEFVAR TF@$count\n");
    printf("MOVE TF@$count int@0\n");
    //character
    printf("DEFVAR TF@$char\n");
    //output string
    printf("DEFVAR TF@$ret\n");
    printf("MOVE TF@$ret string@\n");


    //if empty string or i <=0
    printf("DEFVAR TF@$tmp_len\n");
    printf("STRLEN TF@$tmp_len TF@$in_str\n");
    printf("JUMPIFEQ &end%d TF@$tmp_len int@0\n", name);

    printf("DEFVAR TF@$tmp_bool\n");
    printf("GT TF@$tmp_bool TF@$start_pos int@-1\n");
    printf("JUMPIFNEQ &end%d TF@$tmp_bool bool@true\n", name);
    
    //if n < 0 or n > length(string)
    printf("LT TF@$tmp_bool TF@$length int@0\n");
    printf("JUMPIFEQ &cycle2%d TF@$tmp_bool bool@true\n", name);

    printf("GT TF@$tmp_bool TF@$length TF@$tmp_len\n");
    printf("JUMPIFEQ &cycle2%d TF@$tmp_bool bool@true\n", name);


    ////////////////////cycle1
    printf("LABEL &cycle1%d\n", name);
    printf("GETCHAR TF@$char TF@$in_str TF@$start_pos\n");
    //conect pieces of return value
    printf("CONCAT TF@$ret TF@$ret TF@$char\n");
    printf("ADD TF@$count TF@$count int@1\n");
    printf("ADD TF@$start_pos TF@$start_pos int@1\n");

    //if counter == length i jump to end label
    printf("JUMPIFNEQ &cycle1%d TF@$count TF@$length\n", name);
    printf("JUMP &end%d\n", name);

    printf("LABEL &cycle2%d\n", name);
    printf("SUB TF@$length TF@$tmp_len TF@$start_pos\n");
    printf("JUMP &cycle1%d\n", name);
    

    printf("LABEL &end%d\n", name);
    printf("PUSHS TF@$ret\n");

    return true;
}
