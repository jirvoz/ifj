#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

enum tstates {
	BEGIN,
	MINUS,				//1 - minus
	//IN-COMMENT-CHECK, //2 - kontrola vnoreneho komentare 
	//CHECK 			//3 - kontrola druheho znaku vnoreneho komentare
	SINGLE-LINE-COMMENT		//4 - comment on 1 line
	MULTI-LINE-COMMENT	//5 - multi line comment
	END-OF-COMMENT
	//END-IN-COMMENT, 	//6 - konec vnoreneho komentare
	IDENTIFIER,		//7 - identifikator
	STRING,				//8 - retezec
	ESCAPE-SEQUENCE,	//9 - kontrola escape sekvenci
	ZERO-NUMBER			//delete first ZEROS in the number
	NUMBER, 				//10 - vynechani pocatecnich nul u cisla
	EXPONENT,	//11 - celociselna cast cisla
	UNEMPTY-CHECK, //12 - kontrola neprazdnosti desetineho cisla
	FLOATING-POINT,	//13 - desetine cislo
	POWER-CHECK,	//14 - kontrola neprazdnosti mocniny nebo zadani +- u mocniny
	PLUSMINUS-CHECK,			//15 - kontrola neprazdnosti cisla mocniny po zadani +-
	ZERO-CHECK,		//16 - vynechani pocatecnich nul u mocniny
	POWER-VALUE,				//17 - hodnoty mocniny
	CONCATENATION,		//18 - konkatenace
	SINGLE-OPERATOR,//19 - rozliseni dvouznakovych operatoru od jednoznakovych == <= >= 
	MULTI-OPERATOR ,//20 - kontrola spravnosti dvouznakoveho operatoru ~=
}

#define SUCCESS 1
#define FAILURE 0
#define APOSTROPHE 39
#define BACKSLASH 92


