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
typedef enum states {
    BEGIN,                      //0 _ initial state
    SINGLE_LINE_COMMENT,        //1 _ comment on 1 line
    MULTI_LINE_COMMENT,         //2 _ multi line comment
    END_OF_COMMENT,             //3 _ valid end of multi line comment
    IDENTIFIER_KEY,             //4 _ identifier or (reserved)key word
    IS_STRING,                  //5 _ string
    ESCAPE_SEQUENCE,            //6 _ escape sequence
    ESCAPE_NUMBER,              //
    FLOATING_EXPONENT,          //7 _ floating point in exponent
    NUMBER,                     //9 _ number
    EXPONENT,                   //10 _ exponent
    FLOATING,                   //11 _ number is floating point
    LOWER,                      //12 _ lower operator _ '<'
    HIGHER ,                    //13 _ higher operator _ '>'
} automata_states;

//types of TOKENS sent to parser
typedef enum tokens {
    SINGLE_OPERATOR,
    DOUBLE_OPERATOR,
    END_OF_FILE,
    KEY_WORD,
    RESERVED_KEY_WORD,
    IDENTIFIER,
    STRING,
    INTEGER,
    FLOATING_POINT,
    FLOATING_POINT_EXPONENT,
} token_types;

//Declarations of functions
<<<<<<< HEAD
int getNextToken (string*, token_types*, FILE*);
=======
int getNextToken(string*, enum tokens*, FILE*);
>>>>>>> 94f5e046dc96bc6af1d2394ff455212632aa5d30
