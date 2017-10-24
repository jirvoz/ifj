#include <stdio.h>
#include "strings.h"

//codes from ASCII table
#define APOSTROPHE 39
#define BACKSLASH 92
#define QUOTE 34

//Other constants
#define KEYWORDS 22             //number of keywords
#define RESERVED_KEYWORDS 13    //number of reserved keywords
#define MEM_ERROR 1
#define LEX_ERROR 51 
#define OK 7
#define SUCCESS 0               //bash style
#define FAILURE 1               //bash style

// states for finite automata
typedef enum automata_states {
    BEGIN,                      //0 - initial state
    SINGLE_LINE_COMMENT,        //1 - comment on 1 line
    MULTI_LINE_COMMENT,         //2 - multi line comment
    END_OF_COMMENT,             //3 - valid end of multi line comment
    IDENTIFIER_KEY,             //4 - identifier or (reserved)keyword
    IS_STRING,                  //5 - string
    NUMBER,                     //6 - number
    FLOATING,                   //7 - number is floating point
    FLOATING_EXPONENT,          //8 - floating point in exponent
    EXPONENT,                   //9 - exponent
    LOWER,                      //10 - lower operator - '<'
    HIGHER ,                    //11 - higher operator - '>'
    ESCAPE_SEQUENCE,            //12 - escape sequence
    ESCAPE_NUMBER               //13 - \ddd number in escape sequence
} automata_states;

//types of TOKENS sent to parser
typedef enum tokens_types {
    SINGLE_OPERATOR,
    DOUBLE_OPERATOR,
    END_OF_FILE,
    IDENTIFIER,
    STRING,
    INTEGER,
    FLOATING_POINT,
    FLOATING_POINT_EXPONENT,
    //------------KEYWORDS-------------//
    AS = 10,                    //starting at 10
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
    //-------RESERVED-KEYWORDS---------//
    AND = 40,                   //starting at 40       
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
} token_types;
    
//Declarations of functions
int getNextToken (string*, token_types*, FILE*);    