#include <ctype.h>
#include <stdlib.h>
#include "scanner.h"

unsigned line = 1;      //line counter
int next = -1;          //character in queue, if -1, no character is in queue

char* keywords[KEYWORDS] = {
    "as", "asc", "declare", "dim", "do", "double", "else", "end", "chr",
    "function", "if", "input", "integer", "length", "loop", "print", "return",
    "scope", "string", "substr", "then", "while"
};

char* reserved_keywords[RESERVED_KEYWORDS] = {
    "and", "boolean", "continue", "elseif", "exit", "false", "for", "next",
    "not", "or", "shared", "static", "true"
};

int operatorTest(char c) {
    token_types type;
    
    if (c == '+') {
        type = PLUS_OP;
    }
    else if (c == '-') {
        type = MINUS_OP;
    }
    else if (c == '*') {
        type = STAR_OP;
    }
    else if (c == BACKSLASH) {
        type = BACKSLASH_OP;
    }
    else if (c == '(') {
        type = LEFT_PARENTH_OP;
    }
    else if (c == ')') {
        type = RIGHT_PARENTH_OP;
    }
    else if (c == '=') {
        type = EQUAL_SIGN_OP;
    }
    else if (c == ',') {
        type = COLON_OP;
    }
    else if (c == ';') {
        type = SEMICOLON_OP;
    }
    else {
        return -1;
    }

    return ((int) type);
}

int getNextToken (token* next_token, FILE* source_file) {
    string* tmp_string = (string*) malloc (sizeof(string));
    stringInit(tmp_string);
    next_token->attribute.identifier_string = tmp_string;

    int int_tmp;
    unsigned escape_number;
    automata_states state = BEGIN;          //first state is BEGIN
    char c;                                 //lexem

    //just for tests, will be deleted later
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
                if (!stringAddChar(c, tmp_string)) {
                    state = IDENTIFIER_KEY;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '!') {
                if ((c = getc(source_file)) == QUOTE) {
                    state = IS_STRING;
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
                    next = c;
                    next_token->type = SLASH_OP;
                    return OK;
                }
            }
            else if (isdigit(c)) {
                if (!stringAddChar(c, tmp_string)) {
                    state = NUMBER;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            //single operators 
            else if ((int_tmp = operatorTest(c)) && int_tmp != -1) {
                next_token->type = int_tmp;
                return OK;
            }
            else if (c == '<') {
                state = LOWER;   
            }
            else if (c == '>') {
                state = HIGHER;
            }
            else if (c == '\n') {
                line++;
            }
            else if (isspace(c)) {
                //nothing to do, white characters are ignored
            }
            else if (c = EOF) {
                next_token->type = END_OF_FILE;
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
                next_token->type = END_OF_FILE;
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
                next_token->type = END_OF_FILE;
                return LEX_ERROR;
            }
            //we ignore comments
        }

/*********************************IDENTIFIER_KEY STATE************************************/

        else if (state == IDENTIFIER_KEY) {
            if (isalnum(c) || c == '_') {
                if (!stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else {
                next = c;
                state = BEGIN;

                if ((int_tmp = identifierTest(tmp_string, keywords, reserved_keywords)) && int_tmp != -1) {
                    next_token->type = int_tmp;
                    return OK;
                }
                else {
                    next_token->type = IDENTIFIER;
                    return OK;
                }
            }
        }

/*********************************OPERATOR LOWER STATE************************************/

        else if (state == LOWER) {
            if (c == '>') {
                next_token->type = NO_EQUAL_OP;
                return OK;
            }
            else if (c == '=') {
                next_token->type = LOWER_EQUAL_OP;
                return OK;
            }
            else {
                next = c;
                next_token->type = LOWER_OP;
                return OK;
            }
        }

/*********************************OPERATOR HIGHER STATE************************************/

        else if (state == HIGHER) {
            if (c == '=') {
                next_token->type = HIGHER_EQUAL_OP;
                return OK;
            }
            else {
                next = c;
                next_token->type = HIGHER_OP;
                return OK;
            }
        }

/***********************************STRING STATE*******************************************/

        else if (state == IS_STRING) {
            if (c == QUOTE) {
                next_token->type = STRING_TOK;
                return OK;
            }
            else if (c > 31 && c <= 255) {
                if (!stringAddChar(c, tmp_string)) {
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
                next_token->type = END_OF_FILE;
                return LEX_ERROR;
            }
            else {
                addError(line, LEX_ERROR);
                next_token->type = STRING_TOK;
                return LEX_ERROR;
            }
        }

/*********************************NUMBER STATE************************************/

        else if (state == NUMBER) {
            if (isdigit(c)) {
                if (!stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '.') {
                if (!stringAddChar(c, tmp_string)) {
                    state = FLOATING_POINT;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (!stringAddChar(c, tmp_string)) {
                    state = EXPONENT;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else {
                next = c;
                next_token->attribute.number = strtol(tmp_string->str, NULL, 10);
                stringFree(tmp_string);
                free(tmp_string);
                next_token->type = INTEGER_TOK;
                return OK;
            }
        }

/*********************************FLOATING_POINT STATE************************************/

        else if (state == FLOATING_POINT) {
            if (isdigit(c)) {
                if (!stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (!stringAddChar(c, tmp_string)) {
                    state = EXPONENT;

                    if ((c = getc(source_file) && c == '+') || c == '-'){
                        if (!stringAddChar(c, tmp_string)) {
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
                next_token->attribute.float_number = strtod(tmp_string->str, NULL);
                stringFree(tmp_string);
                free(tmp_string);
                next_token->type = FLOATING_POINT;
                return OK;
            }
        }

/*********************************EXPONENT STATE************************************/

        else if (state == EXPONENT) {
            if (isdigit(c)) {
                if (!stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == '.') {
                if (!stringAddChar(c, tmp_string)) {
                    state = FLOATING_EXPONENT;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else {
                next = c;
                next_token->attribute.float_number = strtod(tmp_string->str, NULL);
                stringFree(tmp_string);
                free(tmp_string);
                next_token->type = FLOATING_POINT_EXPONENT;
                return OK;
            }
        }

/*********************************FLOATING_EXPONENT STATE************************************/

        else if (state == FLOATING_EXPONENT) {
            if (isdigit(c)) {
                if (!stringAddChar(c, tmp_string)) {
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else {
                next = c;
                next_token->attribute.float_number = strtod(tmp_string->str, NULL);
                stringFree(tmp_string);
                free(tmp_string);
                next_token->type = FLOATING_POINT_EXPONENT;
                return OK;
            }
        }

/*********************************ESCAPE_SEQUENCE STATE************************************/

        else if (state == ESCAPE_SEQUENCE) {
            if (c == QUOTE) {
                if (!stringAddChar(QUOTE, tmp_string)) {
                    state = IS_STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'n') {
                if (!stringAddChar('\n', tmp_string)) {
                    state = IS_STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 't') {
                if (!stringAddChar('\t', tmp_string)) {
                    state = IS_STRING;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == BACKSLASH) {
                if (!stringAddChar(BACKSLASH, tmp_string)) {
                    state = IS_STRING;
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
                    if (!stringAddChar(escape_number, tmp_string)) {
                        state = IS_STRING;
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
