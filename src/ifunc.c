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
#include "functions.h"
#include "expressions.h"

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

    ////////////////////in case of invalid number
    printf("LABEL $retnull\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$ret0\n");
    printf("MOVE LF@$ret0 int@0\n");
    printf("PUSHS LF@$ret0\n");
    printf("JUMP $ascend\n");

    ////////////////////Asc function
    printf("LABEL $Asc\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$ret\n");
    printf("DEFVAR LF@$pos\n");
    printf("DEFVAR LF@$str1\n");
    //var for testing
    printf("DEFVAR LF@$len\n");
    printf("DEFVAR LF@$int1\n");
    //var for bool
    printf("DEFVAR LF@$bool1\n");
    printf("DEFVAR LF@$bool2\n");
    //set e to int 1
    printf("MOVE LF@$int1 int@1\n");
    //set bool values
    printf("MOVE LF@$bool1 bool@false\n");
    printf("MOVE LF@$bool2 bool@true\n");
    //position
    printf("POPS LF@$pos\n");
    //string
    printf("POPS LF@$str1\n");
    //put lenght of string to d
    printf("STRLEN LF@$len LF@$str1\n");
    //we need dec by 1 becouse we count from 0
    printf("SUB LF@$len LF@$len LF@$int1\n");
    //test if position isn't greater than lenght of str
    printf("GT LF@$bool1 LF@$pos LF@$len\n");
    printf("JUMPIFEQ $retnull LF@$bool1 LF@$bool2\n");
    //STRI2INT <var> <string> <position(int)>
    printf("STRI2INT LF@$ret LF@$str1 LF@$pos\n");
    printf("PUSHS LF@$ret\n");
    printf("JUMP $ascend\n");

    ////////////////////Asc end
    printf("LABEL $ascend\n");
    printf("POPFRAME\n");

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

    ////////////////////error label
    printf("LABEL $ndef\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$a\n");
    printf("MOVE LF@$a int@1\n");
    printf("PUSHS LF@$a\n");
    printf("JUMP $chrend\n");

    ////////////////////Chr function
    printf("LABEL $Chr\n");
    //create frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    //variables
    printf("DEFVAR LF@$ret\n");
    printf("DEFVAR LF@$int1\n");
    //var for controll
    printf("DEFVAR LF@$c\n");
    printf("MOVE LF@$c int@0\n");

    //var for bool value
    printf("DEFVAR LF@$bool1\n");
    printf("DEFVAR LF@$bool2\n");
    printf("MOVE LF@$bool2 bool@false\n");
    //get in value
    printf("POPS LF@$int\n");
    //check if value is higher than 0
    printf("GT LF@$bool1 LF@$int LF@$c\n");
    printf("JUMPIFEQ $ndef LF@$bool1 LF@$bool2\n");
    //change c from 0 to 255
    printf("MOVE LF@$c int@255\n");
    //check if value is less than 255
    printf("LT LF@$bool1 LF@$int1 LF@$c\n");
    printf("JUMPIFEQ $error LF@$bool1 LF@$bool2\n");
    //change int value to char in ascii      printf("INT2CHAR LF@$ret LF@$int1\n");
    printf("PUSHS LF@$ret\n");
    printf("JUMP $chrend\n");

    ////////////////////end
    printf("LABEL $chrend\n");
    printf("POPFRAME\n");

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

    printf("JUMP $SubStr\n");
    ////////////////////return everting from strart position (refsp)
    printf("LABEL $refsp\n");
    //we must change length to bmsp
    printf("MOVE LF@$length LF@$bmsp\n");
    printf("JUMP $cycle\n");

    ////////////////////cycle
    printf("LABEL $cycle\n");
    printf("GETCHAR LF@$sstr LF@$bstr LF@$spos\n");
    //conect pieces of return value
    printf("CONCAT LF@$ret LF@$bret LF@$sstr\n");
    printf("ADD LF@$count LF@$count LF@$plus1\n");
    //if counter == length i jump to end label
    printf("JUMPIFEQ $ssend LF@$count LF@$length\n");
    printf("JUMP $cycle\n");

    ////////////////////return empty str
    printf("LABEL $res\n");
    //move to return empty str
    printf("MOVE LF@$ret string@\n");
    printf("JUMP $ssend\n");

    ////////////////base func
    printf("LABEL $SubStr\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$ret\n");
    //base str
    printf("DEFVAR LF@$bstr\n");
    //start pos
    printf("DEFVAR LF@$spos\n");
    //length of sub str
    printf("DEFVAR LF@$length\n");
    //int vars
    printf("DEFVAR LF@$plus1\n");
    printf("DEFVAR LF@$count\n");
    //sub string
    printf("DEFVAR LF@$sstr\n");
    //base str length
    printf("DEFVAR LF@$bsl\n");
    //bool var
    printf("DEFVAR LF@$bool1\n");
    printf("DEFVAR LF@$bool2\n");
    //find out base str length
    printf("STRLEN LF@$bsl LF@bstr\n");
    //bsl minus start position
    printf("DEFVAR LF@$bmsp\n");

    printf("MOVE LF@$bool2 bool@true\n");
    printf("MOVE LF@$plus1 int@1\n");
    printf("MOVE LF@$count int@0\n");

    printf("POPS LF@$length\n");
    printf("POPS LF@$spos\n");
    printf("POPS LF@$bstr\n");
    //if bsl == 0 jump to return empty str (res)
    printf("JUMPIFEQ $res LF@$bsl LF@$count\n");
    //if start pos of subs == 0 jump to return empty str (res)
    printf("JUMPIFEQ $res LF@$spos LF@$count\n");
    //check if start pos isn't negative
    printf("LT LF@$bool1 LF@$spos LF@$count\n");
    printf("JUMPIFEQ $res LF@$bool1 LF@$bool2\n");
    //check if length of sub str isn't negative
    printf("LT LF@$bool1 LF@$length LF@$count\n");
    printf("JUMPIFEQ $refsp LF@$bool1 LF@$bool2\n");
    //length of base str minus start position
    printf("SUB LF@$bmsp LF@$bsl LF@$spos\n");
    //check if length isn't greather than bmsp
    printf("GT LF@$bool1 LF@$length LF@$bmsp\n");
    printf("JUMPIFEQ $refsp LF@$bool1 LF@$bool2\n");

    printf("JUMP $cycle\n");

    //////////////////////substring end
    printf("LABEL $ssend\n");
    printf("PUSHS LF@$ret\n");
    printf("POPFRAME\n");

    return true;
}
