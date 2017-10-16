#include "scanner.h"

unsigned line = 1; 		//line counter

int rest = -1;			//rest character

char* tmp_string;

char* key_words[KEY_WORDS] = {
	"As", "Asc", "Declare", "Dim", "Do", "Double", "Else", "End", "Chr",
	"Function", "If", "Input", "Integer", "Length", "Loop", "Print", "Return",
	"Scope", "String", "SubStr", "Then", "While"
};

char* reserved_key_words[RESERVED_KEY_WORDS] = {
	"And", "Boolean", "Continue", "Elseif", "Exit", "False", "For", "Next",
	"Not", "Or", "Shared", "Static", "True"
};

//add character to 
int addChar (char c, char* my_string) {

	return SUCCESS;
}

int getNextToken(char* attr, int* type, FILE* source_file) {
	string str = "";	//read string
	int next = -1;
	unsigned escape_number;
	states state = BEGIN;
	char c;

	do {
		if (next == -1){
			c = getc(source_file);
		}
		else {
			c = next;
			next = -1;
		}
		
		if (state == BEGIN) {
			if (isalpha(c) || c == '_') {
				if (addChar(c, tmp_string)) {
					state = IDENTIFIER-KEY;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '!') {
				state = STRING;
			}
			else if (c == APOSTROPHE) {						
				state = SINGLE-LINE-COMMENT;
			}
			else if (c == '/') {	
				if (getc(c, source_file) == APOSTROPHE) {
					state = MULTI-LINE-COMMENT;
				}								
				else {
					if (addChar(c, tmp_string)) {
						*type = SINGLE_OPERATOR;
						return OK;
					}
					else {
						addError(line, errors);
						return MEM_ERROR;
					}

				}
			}
			else if (isdigit(c)) {									
				if (addChar(c, tmp_string)) {
					state = NUMBER;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '+' || c == '-' || c == '*' || c == BACKSLASH || c == '(' || c == ')' c == ';' || c == '=') {								
				if (addChar(c, tmp_string)) {
					*type = SINGLE_OPERATOR;
					return OK;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '<') {		
				if (addChar(c, tmp_string)) {
					state = LOWER;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '>') {
				if (addChar(c, tmp_string)) {
					state = HIGHER;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '\n') {
				line++;
			}
			else if (isspace(c)) {
				//nothing to do, white characters are ignored
			}
			else if (c = EOF) {
				*type = END_OF_FILE;
				return OK; 
			}
			else {
				addError(line, lex_errors);
				return LEX_ERROR; 
			}
		}
		else if (state == SINGLE-LINE-COMMENT) {
			if (c == '\n') {	
				line++;
				state = BEGIN;
			}
			else if (c == EOF) {
				*type = END_OF_FILE;
				return OK;
			}
			//we ignore comments
		}
		else if (state == MULTI-LINE-COMMENT) {
			if (c == APOSTROPHE) {
				if (getc(c, source_file) == '/') {
					state = BEGIN;
				}
				else {
					next = c;
				}
			}
			else if (c == '\n') {
				line++;
			}
			else if (c == EOF) {
				//unexpected
				addError(line, lex_errors);
				*type = END_OF_FILE;
				return LEX_ERROR;
			}
			//we ignore comments
		}
		else if (state == IDENTIFIER-KEY) {
			if (isalnum(c) || c == '_') {
				if (addChar(c, tmp_string)) {
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else {
				next = c;
				state = BEGIN;

				if (identifierTest(tmp_string, key_words)) {
					*type = KEY_WORD;
					return OK;
				}
				else if (identifierTest(tmp_string, reserved_key_words)) {
					*type = RESERVED_KEY_WORD;
					return OK;
				}
				else {
					*type = IDENTIFIER;
					return OK;
				}
			}
		}
		else if (state == LOWER) {
			if (c == '>') {
				if (addChar(c, tmp_string)) {
					*type = DOUBLE_OPERATOR;
					return OK;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '=') {
				if (addChar(c, tmp_string)) {
					*type = DOUBLE_OPERATOR;
					return OK;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else {
				next = c;
				*type = SINGLE_OPERATOR;
				return OK;
			}
		}
		else if (state == HIGHER) {
			if (c == '=') {
				if (addChar(c, tmp_string)) {
					*type = DOUBLE_OPERATOR;
					return OK;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else {
				next = c;
				*type = SINGLE_OPERATOR;
				return OK;
			}
		}
		else if (state == STRING-TEST) {
			if (c == QUOTE) {
				state = STRING;
			}
			else {
				//error
			}
		}
		else if (state == STRING) {
			if (c == QUOTE) {
				*type = STRING;
				return OK;
			}
			else if (c > 31 && c <= 255) {
				if (addChar(c, tmp_string)) {
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == BACKSLASH) {
				state = ESCAPE-SEQUENCE;
			}
			else {
				//error
			}
		}
		else if (state == NUMBER) {
			if (isdigit(c)) {
				if (addChar(c, tmp_string)) {
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '.') {
				if (addChar(c, tmp_string)) {
					state = FLOATING-POINT;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == 'e' || c == 'E') {
				if (addChar(c, tmp_string)) {
					state = EXPONENT;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
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
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == 'e' || c == 'E') {
				if (addChar(c, tmp_string)) {
					state = EXPONENT;

					if (getc(c, source_file) == '+' || c == '-'){
						if (addChar(c, tmp_string)) {
						}
						else {
							addError(line, errors);
							return MEM_ERROR;
						}
					}
					else {
						next = c;
					}
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else {
				next = c;
				*type = FLOATING_POINT;
				return OK;
			}
		}
		else if (state == EXPONENT) {
			if (isdigit(c)) {
				if (addChar(c, tmp_string)) {
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else if (c == '.') {
				if (addChar(c, tmp_string)) {
					state = FLOATING-EXPONENT;
				}
				else {
					addError(line, errors);
					return MEM_ERROR;
				}
			}
			else {
				next = c;
				*type = FLOATING_POINT_EXPONENT;
				return OK;
			}
		}
		else if (state == FLOATING-EXPONENT) {
			if (isdigit(c)) {
				if (addChar(c, tmp_string)) {
				}
				else {
					//error...
				}
			}
			else {
				next = c;
				*type = FLOATING_POINT_EXPONENT;
				return OK;
			}
		}
		else if (state == ESCAPE-SEQUENCE) {
			if (c == '"') {
				if (addChar(QUOTE, tmp_string)) {
					state = STRING;
				}
				else {
					//error...
				}
			}
			else if (c == 'n') {
				if (addChar('\n', tmp_string)) {
					state = STRING;
				}
				else {
					//error...
				}
			}
			else if (c == 't') {
				if (addChar('\t', tmp_string)) {
					state = STRING;
				}
				else {
					//error...
				}
			}
			else if (c == BACKSLASH) {
				if (addChar(BACKSLASH, tmp_string)) {
					state = STRING;
				}
				else {
					//error...
				}
			}
			else if (isdigit(c)) {
				escape_number = c - 48;				//to get number from ascii table
				state = ESCAPE-NUMBER;
			}
			else {
				//error
			}
		}
		else if (state == ESCAPE-NUMBER) {
			if (isdigit(c)) {
				escape_number = escape_number*10 + (c-48);

				c = getchar();
				if (isdigit(c)) {
					escape_number = escape_number*10 + (c-48);
					if (addChar(escape_number, tmp_string)) {
						state = STRING;
					}
					else {
						//error...
					}
				}
				else {
					//error escape number
				}
			}
			else {
				//error escape number
			}
		}
	}

}