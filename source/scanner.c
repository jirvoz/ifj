#include "scanner.h"

unsigned line = 1; 		//line counter

int rest = -1;			//rest character

char* tmp_string;

char** key_words = {
	"As", "Asc", "Declare", "Dim", "Do", "Double", "Else", "End", "Chr",
	"Function", "If", "Input", "Integer", "Length", "Loop", "Print", "Return",
	"Scope", "String", "SubStr", "Then", "While"
};

char** reserved_key_words = {
	"And", "Boolean", "Continue", "Elseif", "Exit", "False", "For", "Next",
	"Not", "Or", "Shared", "Static", "True"
};

//add character to 
int addChar (char c, char* my_string) {

	return SUCCESS;
}

int getNextToken(string *attr, int *type, FILE* source) {
	string str = "";	//read string
	int next = -1;
	unsigned state = BEGIN;
	char symbol;

	do {
		symbol = getchar();
		
		if (state == START) {
			if (isalpha(c) || c == '_') {
				if (addChar(c, tmp_string)) {
					state = IDENTIFIER;
				}
				else {
					//error...
				}
			}
			else if (c == '!') {
				state = STRING;
			}
			else if (c == APOSTROPHE) {						
				state = SINGLE-LINE-COMMENT;
			}
			else if (c == '/') {									
				state = MULTI-LINE-COMMENT;
			}
			else if (isdigit(c)) {									
				if (addChar(c, tmp_string)) {
					state = NUMBER;
				}
				else {
					//error...
				}
			}
			else if (c == '+' || c == '-' || c == '*' || c == '/' || c == APOSTROPHE || c == '(' || c == ')') {								
				if (addChar(c, tmp_string)) {
					state = SINGLE-OPERATOR;
				}
				else {
					//error...
				}
			}
			else if (c == '<' || c == '>' || c == '=') {		
				if (addChar(c, tmp_string)) {
					state = MULTI-OPERATOR;
				}
				else {
					//error...
				}
			}
			else if (c = EOF) {
				//neco
			}
			else {
				//error...
			}
		}
		else if (state == NUMBER) {

		}
		else if (state == SINGLE-LINE-COMMENT) {
			if (c == '\n') {	
				state = BEGIN;
			}
			else if (c == EOF) {
				//neco
			}
			//we don't store comments
		}
		else if (state == MULTI-LINE-COMMENT) {
			if (c == 39) {
				state = END-OF-COMMENT;
			}
			else if (c == EOF) {
				//neco
			}
		}
		else if (state == END-OF-COMMENT) {
			if (c == '/') {
				state = BEGIN;
			}
			else if (c == EOF) {
				//neco
			}
			else {
				state = MULTI-LINE-COMMENT;
			}
		}
		else if (state == IDENTIFIER) {

		}
		else if (state == OPERATOR) {

		}
		else if (state == OPERATORS) {

		}
		else if (state == STRING) {

		}
		else if (state == NUMBER) {
			if (isdigit(c)) {
				if (addChar(c, tmp_string)) {
				}
				else {
					//error...
				}
			}
			else if (c == '.') {
				if (addChar(c, tmp_string)) {
					state = FLOATING-POINT;
				}
				else {
					//error...
				}
			}
			else if (c == 'e' || c == 'E') {
				if (addChar(c, tmp_string)) {
					state = EXPONENT;
				}
				else {
					//error...
				}
			}
			else {
				next = c;
				*type = INTEGER;
				return OK;
			}
		}
		else if (state == FLOATING-POINT) {
			if (isdigit(c)) {
				if (addChar(c, tmp_string)) {
				}
				else {
					//error...
				}
			}
			else if (c == 'e' || c == 'E') {
				if (addChar(c, tmp_string)) {
					state = EXPONENT;
				}
				else {
					//error...
				}
			}
			else {
				next = c;
				*type = FLOATING-POINT;
				return OK;
			}
		}
		else if (state == EXPONENT) {
			
		}
	}

}