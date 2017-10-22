#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"

// states for finite automata
enum states {
	BEGIN,					//0 _ initial state
	SINGLE_LINE_COMMENT,	//1 _ comment on 1 line
	MULTI_LINE_COMMENT,		//2 _ multi line comment
	END_OF_COMMENT,			//3 _ valid end of multi line comment
	IDENTIFIER_KEY,			//4 _ identifier or (reserved)key word
	IS_STRING,					//5 _ string
	ESCAPE_SEQUENCE,		//6 _ escape sequence
	ESCAPE_NUMBER,
	FLOATING_EXPONENT,		//7 _ floating point in exponent
	NUMBER, 				//9 _ number
	EXPONENT,				//10 _ exponent
	FLOATING,			//11 _ number is floating point
	LOWER,					//12 _ lower operator _ '<'
	HIGHER ,				//13 _ higher operator _ '>'
};

//some codes from ASCII table
#define SUCCESS 1
#define FAILURE 0
#define APOSTROPHE 39
#define BACKSLASH 92
#define QUOTE 34

//types of TOKENS
enum tokens {
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
};

//Other constants
#define KEY_WORDS 22
#define RESERVED_KEY_WORDS 13
#define MEM_ERROR 1
#define LEX_ERROR 5
#define OK 7

//Declarations of functions
int getNextToken(string*, enum tokens*, FILE*);
