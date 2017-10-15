#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

enum tstates {
	BEGIN,
	MINUS,				//1 - komentar/minus
	IN-COMMENT-CHECK, //2 - kontrola vnoreneho komentare 
	CHECK 			//3 - kontrola druheho znaku vnoreneho komentare
	COMMENT			//4 - komentar
	IN-COMMENT,	// 5 - vnoreny komentar
	END-IN-COMMENT, 	//6 - konec vnoreneho komentare
	IDENTIFIER,		//7 - identifikator
	STRING,				//8 - retezec
	ESCAPE-SEQUENCE,	//9 - kontrola escape sekvenci
	NUMBER, 				//10 - vynechani pocatecnich nul u cisla
	INT-PART,	//11 - celociselna cast cisla
	UNEMPTY-CHECK, //12 - kontrola neprazdnosti desetineho cisla
	FLOAT-NUMBER,	//13 - desetine cislo
	POWER-CHECK,	//14 - kontrola neprazdnosti mocniny nebo zadani +- u mocniny
	PLUSMINUS-CHECK,			//15 - kontrola neprazdnosti cisla mocniny po zadani +-
	ZERO-CHECK,		//16 - vynechani pocatecnich nul u mocniny
	POWER-VALUE,				//17 - hodnoty mocniny
	CONCATENATION,		//18 - konkatenace
	OPERATORS,//19 - rozliseni dvouznakovych operatoru od jednoznakovych == <= >= 
	TWO-CHAR-OPERATOR ,//20 - kontrola spravnosti dvouznakoveho operatoru ~=
}

