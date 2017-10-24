#include "scanner.h"
#include "ctype.h"

unsigned line = 1;      //line counter

int next = -1;          //character in queue, if -1,no character is in queue

char* keywords[KEYWORDS] = {
    "As", "Asc", "Declare", "Dim", "Do", "Double", "Else", "End", "Chr",
    "Function", "If", "Input", "Integer", "Length", "Loop", "Print", "Return",
    "Scope", "String", "SubStr", "Then", "While"
};

char* reserved_keywords[RESERVED_KEYWORDS] = {
    "And", "Boolean", "Continue", "Elseif", "Exit", "False", "For", "Next",
    "Not", "Or", "Shared", "Static", "True"
};

int getNextToken (string* tmp_string, token_types* type, FILE* source_file) {
    unsigned escape_number;
    automata_states state = BEGIN;          //first state is BEGIN
    char c;                             //lexem
    int errors = 1; 
    int lex_errors = 2;

    do {
        if (next == -1){
            c = getc(source_file);      //get lexem from source file
        }
        else {
            c = next;
            next = -1;
        }

        if (isalpha(c) && isupper(c)) {
            c = tolower(c);             // IFJCODE17 is case insensitive
        }

/*********************************BEGIN STATE************************************/
        if (state == BEGIN) {
            if (isalpha(c) || c == '_') {
                if (stringAddChar(c, tmp_string)) {
                    state = IDENTIFIER_KEY;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '!') {
                if (c = getc(source_file) && c == QUOTE) {
                    state = STRING;
                }
                else {
                    next = c;
                    addError(line, LEX_ERROR);
                    return LEX_ERROR;
                }

            }
            else if (c == APOSTROPHE) {
                state = SINGLE_LINE_COMMENT;
            }
            else if (c == '/') {
                if (c = getc(source_file) && c == APOSTROPHE) {
                    state = MULTI_LINE_COMMENT;
                }
                else {
                    if (stringAddChar('/', tmp_string)) {
                        next = c;
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
                if (stringAddChar(c, tmp_string)) {
                    state = NUMBER;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '+' || c == '_' || c == '*' || c == BACKSLASH || c == '(' || c == ')' || c == ';' || c == '=' || c == ',') {
                if (stringAddChar(c, tmp_string)) {
                    *type = SINGLE_OPERATOR;
                    return OK;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '<') {
                if (stringAddChar(c, tmp_string)) {
                    state = LOWER;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '>') {
                if (stringAddChar(c, tmp_string)) {
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
/*********************************SINGLE_LINE COMMENT STATE************************************/
        else if (state == SINGLE_LINE_COMMENT) {
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
/*********************************MULTI_LINE COMMENT STATE************************************/
        else if (state == MULTI_LINE_COMMENT) {
            if (c == APOSTROPHE) {
                if (c = getc(source_file) && c == '/') {
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
/*********************************IDENTIFIER_KEY STATE************************************/
        else if (state == IDENTIFIER_KEY) {
            if (isalnum(c) || c == '_') {
                if (stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else {
                next = c;
                state = BEGIN;

                if (identifierTest(tmp_string, keywords)) {
                    *type = KEY_WORD;
                    return OK;
                }
                else if (identifierTest(tmp_string, reserved_keywords)) {
                    *type = RESERVED_KEY_WORD;
                    return OK;
                }
                else {
                    *type = IDENTIFIER;
                    return OK;
                }
            }
        }
/*********************************OPERATOR LOWER STATE************************************/
        else if (state == LOWER) {
            if (c == '>') {
                if (stringAddChar(c, tmp_string)) {
                    *type = DOUBLE_OPERATOR;
                    return OK;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '=') {
                if (stringAddChar(c, tmp_string)) {
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
/*********************************OPERATOR HIGHER STATE************************************/
        else if (state == HIGHER) {
            if (c == '=') {
                if (stringAddChar(c, tmp_string)) {
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
/*********************************STRING STATE************************************/
        else if (state == STRING) {
            if (c == QUOTE) {
                *type = STRING;
                return OK;
            }
            else if (c > 31 && c <= 255) {
                if (stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == BACKSLASH) {
                state = ESCAPE_SEQUENCE;
            }
            else if (c == EOF) {
                addError(line, LEX_ERROR);
                *type = END_OF_FILE;
                return LEX_ERROR;
            }
            else {
                addError(line, LEX_ERROR);
                return LEX_ERROR;
            }
        }
/*********************************NUMBER STATE************************************/
        else if (state == NUMBER) {
            if (isdigit(c)) {
                if (stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '.') {
                if (stringAddChar(c, tmp_string)) {
                    state = FLOATING_POINT;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (stringAddChar(c, tmp_string)) {
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
/*********************************FLOATING_POINT STATE************************************/
        else if (state == FLOATING_POINT) {
            if (isdigit(c)) {
                if (stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (stringAddChar(c, tmp_string)) {
                    state = EXPONENT;

                    if ((c = getc(source_file) && c == '+') || c == '_'){
                        if (stringAddChar(c, tmp_string)) {
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
/*********************************EXPONENT STATE************************************/
        else if (state == EXPONENT) {
            if (isdigit(c)) {
                if (stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '.') {
                if (stringAddChar(c, tmp_string)) {
                    state = FLOATING_EXPONENT;
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
/*********************************FLOATING_EXPONENT STATE************************************/
        else if (state == FLOATING_EXPONENT) {
            if (isdigit(c)) {
                if (stringAddChar(c, tmp_string)) {
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
/*********************************ESCAPE_SEQUENCE STATE************************************/
        else if (state == ESCAPE_SEQUENCE) {
            if (c == QUOTE) {
                if (stringAddChar(QUOTE, tmp_string)) {
                    state = STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'n') {
                if (stringAddChar('\n', tmp_string)) {
                    state = STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 't') {
                if (stringAddChar('\t', tmp_string)) {
                    state = STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == BACKSLASH) {
                if (stringAddChar(BACKSLASH, tmp_string)) {
                    state = STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (isdigit(c)) {
                escape_number = c - 48;             //to get number from ascii table
                state = ESCAPE_NUMBER;
            }
            else {
                addError(line, LEX_ERROR);
                return LEX_ERROR;
            }
        }
/*********************************ESCAPE_NUMBER STATE************************************/
        else if (state == ESCAPE_NUMBER) {
            if (isdigit(c)) {
                escape_number = escape_number*10 + (c - 48);

                c = getc(source_file);
                if (isdigit(c)) {
                    escape_number = escape_number*10 + (c - 48);
                    if (stringAddChar(escape_number, tmp_string)) {
                        state = STRING;
                    }
                    else {
                        addError(line, errors);
                    return MEM_ERROR;
                    }
                }
                else {
                    addError(line, LEX_ERROR);
                    return LEX_ERROR;
                }
            }
            else {
                addError(line, LEX_ERROR);
                return LEX_ERROR;
            }
        }
    } while (1);

}
