/* *******************************(IFJ 2017)********************************* */
/*  Course:  Formal Languages and Compilers (IFJ) - FIT VUT Brno 2017/18      */
/*  Project: Implementation of the IFJ17 imperative language translator       */
/*  File:    Header file of lexical analyser                                  */
/*                                                                            */
/*  Authors: Tomáš Nereča : xnerec00 : ()% (team leader)                      */
/*           Samuel Obuch : xobuch00 : ()%                                    */
/*           Jiří Vozár   : xvozar04 : ()%                                    */
/*           Ján Farský   : xfarsk00 : ()%                                    */
/* ************************************************************************** */

#include <stdio.h>
#include "strings.h"

//codes from ASCII table
#define APOSTROPHE 39
#define BACKSLASH 92
#define QUOTE 34

//Other constants
#define KWD_COUNT 35             //number of keywords

extern unsigned line;           //extern variable - line counter

//types of TOKENS sent to parser
typedef enum token_type {
    IDENTIFIER_TOK,
    STRING_TOK,
    INTEGER_TOK,
    FLOATING_POINT_TOK,
    EOL_TOK,
    EOF_TOK,
    //------------OPERATORS-------------//
    LOWER_OP = 10,              //starting at 10
    HIGHER_OP,
    EQUAL_SIGN_OP,
    NO_EQUAL_OP,
    LOWER_EQUAL_OP,
    HIGHER_EQUAL_OP,
    PLUS_OP,
    MINUS_OP,
    STAR_OP,
    SLASH_OP,
    BACKSLASH_OP,
    LEFT_PARENTH_OP,
    RIGHT_PARENTH_OP,
    COLON_OP,
    SEMICOLON_OP,
    //------------KEYWORDS-------------//
    AS = 30,                    //starting at 30
    ASC,
    DECLARE,
    DIM,
    DO,
    DOUBLE,
    ELSE,
    END,
    CHR,
    FUNCTION,
    IF,
    INPUT,
    INTEGER,
    LENGTH,
    LOOP,
    PRINT,
    RETURN,
    SCOPE,
    STRING,
    SUBSTR,
    THEN,
    WHILE,
    AND,                          
    BOOLEAN,
    CONTINUE,
    ELSEIF,
    EXIT,
    FALSE,
    FOR,
    NEXT,
    NOT,
    OR,
    SHARED,
    STATIC,
    TRUE
} token_type;

typedef union tToken_attribute {
    int number;
    double float_number;
    char* string_ptr;
} tToken_attribute;

typedef struct tToken {
    token_type type;
    tToken_attribute attribute;
} tToken;
    
//Declarations of functions
int getNextToken (tToken*, FILE*);                   //main functions of scanner
int operatorTest (char);                            //this functions tests, if next token is operator(+,-,...)