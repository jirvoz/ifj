#include "scanner.h"

unsigned line = 1; 		//line counter

int rest = -1;			//rest character

char** key_words = {
	"As", "Asc", "Declare", "Dim", "Do", "Double", "Else", "End", "Chr",
	"Function", "If", "Input", "Integer", "Length", "Loop", "Print", "Return",
	"Scope", "String", "SubStr", "Then", "While"
};

char** reserved_key_words = {
	"And", "Boolean", "Continue", "Elseif", "Exit", "False", "For", "Next",
	"Not", "Or", "Shared", "Static", "True"
};

int getNextToken(string *attr, int *type, FILE* source) {
	string str = "";	//read string
	unsigned state = 0;
	char symbol;

	do {
		symbol = getchar;

	}

}