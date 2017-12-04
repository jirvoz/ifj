//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        scanner.h
//  Description: Header file of lexical analyser
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <stdio.h>
#include "strings.h"
#include "errors.h"

// Codes from ASCII table
#define APOSTROPHE 39
#define BACKSLASH 92
#define QUOTE 34

// Number of keywords
#define KWD_COUNT 35            

// Shortcut for cleaning string and successful return
#define RETURN_TRUE(string) do { stringFree(&string); return true; } while (0)

// Shortcut for cleaning string and printing error message, unsuccess
#define RETURN_FALSE(string, err_code, ...) do { stringFree(&string); addError(err_code, __VA_ARGS__); \
                                        return false; } while (0)

// Extern variable - line counter
extern unsigned line;

// Types of TOKENS sent to parser
typedef enum token_type
{
    UNDEFINED_TOK = 0,          // Non-existent token for functions in parser
    IDENTIFIER_TOK,
    INTEGER_TOK,
    DOUBLE_TOK,
    STRING_TOK,
    EOL_TOK,
    EOF_TOK,
    //------------OPERATORS-------------//
    EQUAL_SIGN_OP = 10,         // Operators starting at 10
    NO_EQUAL_OP,
    LOWER_EQUAL_OP,
    HIGHER_EQUAL_OP,
    LOWER_OP,
    HIGHER_OP,
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
    AND = 30,                   // Keywords starting at 30
    AS,
    ASC,
    BOOLEAN,
    CHR,
    CONTINUE,
    DECLARE,
    DIM,
    DO,
    DOUBLE,
    ELSE,
    ELSEIF,
    END,
    EXIT,
    FALSE,
    FOR,
    FUNCTION,
    IF,
    INPUT,
    INTEGER,
    LENGTH,
    LOOP,
    NEXT,
    NOT,
    OR,
    PRINT,
    RETURN,
    SCOPE,
    SHARED,
    STATIC,
    STRING,
    SUBSTR,
    THEN,
    TRUE,
    WHILE
} token_type;

// Union representing attribute of number, double, string and identifier token
typedef union tToken_attribute
{
    int number;
    double float_number;
    char* string_ptr;
} tToken_attribute;

// Structure representing Token
typedef struct tToken
{
    token_type type;
    tToken_attribute attribute;
} tToken;
    
// This function tests, if next token is operator(+,-,...)
int operatorTest(char c);
// This function tests, if identifier is keyword
int identifierTest(string* identifier);
// Main function of scanner - implemented as a finite state automata
int getNextToken(tToken* next_token, FILE* source_file);

#endif
