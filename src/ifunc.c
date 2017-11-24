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

    ////////////////base function
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@$ret\n");
    //base str
    printf("DEFVAR TF@$bstr\n");
    //start pos
    printf("DEFVAR TF@$spos\n");
    //length of sub str
    printf("DEFVAR TF@$length\n");
    //int vars
    printf("DEFVAR TF@$plus1\n");
    printf("DEFVAR TF@$count\n");
    //sub string
    printf("DEFVAR TF@$sstr\n");
    //base str length
    printf("DEFVAR TF@$bsl\n");
    //bool var
    printf("DEFVAR TF@$bool1\n");
    printf("DEFVAR TF@$bool2\n");
    //find out base str length
    printf("STRLEN TF@$bsl TF@bstr\n");
    //bsl minus start position
    printf("DEFVAR TF@$bmsp\n");

    printf("MOVE TF@$bool2 bool@true\n");
    printf("MOVE TF@$plus1 int@1\n");
    printf("MOVE TF@$count int@0\n");

    printf("POPS TF@$length\n");
    printf("POPS TF@$spos\n");
    printf("POPS TF@$bstr\n");
    //if bsl == 0 jump to return empty str (res)
    printf("JUMPIFEQ $res TF@$bsl TF@$count\n");
    //if start pos of subs == 0 jump to return empty str (res)
    printf("JUMPIFEQ $res TF@$spos TF@$count\n");
    //check if start pos isn't negative
    printf("LT TF@$bool1 TF@$spos TF@$count\n");
    printf("JUMPIFEQ $res TF@$bool1 TF@$bool2\n");
    //check if length of sub str isn't negative
    printf("LT TF@$bool1 TF@$length TF@$count\n");
    printf("JUMPIFEQ $refsp TF@$bool1 TF@$bool2\n");
    //length of base str minus start position
    printf("SUB TF@$bmsp TF@$bsl TF@$spos\n");
    //check if length isn't greather than bmsp
    printf("GT TF@$bool1 TF@$length TF@$bmsp\n");
    printf("JUMPIFEQ $refsp TF@$bool1 TF@$bool2\n");
    printf("JUMP $cycle\n");

    ////////////////////return everting from strart position (refsp)
    printf("LABEL $refsp\n");
    //we must change length to bmsp
    printf("MOVE TF@$length TF@$bmsp\n");
    printf("JUMP $cycle\n");

    ////////////////////cycle
    printf("LABEL $cycle\n");
    printf("GETCHAR TF@$sstr TF@$bstr TF@$spos\n");
    //conect pieces of return value
    printf("CONCAT TF@$ret TF@$bret TF@$sstr\n");
    printf("ADD TF@$count TF@$count TF@$plus1\n");
    //if counter == length i jump to end label
    printf("JUMPIFEQ $ssend TF@$count TF@$length\n");
    printf("JUMP $cycle\n");

    ////////////////////return empty str
    printf("LABEL $res\n");
    //move to return empty str
    printf("MOVE TF@$ret string@\n");
    printf("JUMP $ssend\n");

    //////////////////////substring end
    printf("LABEL $ssend\n");
    printf("PUSHS TF@$ret\n");

    return true;
}
