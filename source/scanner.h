#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

// states for finite automata
enum states {
	BEGIN,					//0 - initial state
	SINGLE-LINE-COMMENT,	//1 - comment on 1 line
	MULTI-LINE-COMMENT,		//2 - multi line comment
	END-OF-COMMENT,			//3 - valid end of multi line comment
	IDENTIFIER-KEY,			//4 - identifier or (reserved)key word
	STRING,					//5 - string
	ESCAPE-SEQUENCE,		//6 - escape sequence
	FLOATING-EXPONENT,		//7 - floating point in exponent
	NUMBER, 				//9 - number
	EXPONENT,				//10 - exponent
	FLOATING-POINT,			//11 - number is floating point
	LOWER,					//12 - lower operator - '<'
	HIGHER ,				//13 - higher operator - '>'
}

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
}

//Other constants
#define KEY_WORDS 22
#define RESERVED_KEY_WORDS 13

//Declarations of functions
int getNextToken(char*, int*, FILE*);