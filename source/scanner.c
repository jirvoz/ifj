#include <ctype.h>
#include <stdlib.h>
#include "scanner.h"

unsigned line = 1;      //line counter
int next = -1;          //character in queue, if -1, no character is in queue

// states for finite automata

typedef enum automata_state {
    BEGIN_STATE,                      //0 - initial state
    SINGLE_LINE_COMMENT_STATE_STATE,        //1 - comment on 1 line
    MULTI_LINE_COMMENT_STATE,         //2 - multi line comment
    END_OF_COMMENT_STATE_STATE,             //3 - valid end of multi line comment
    IDENTIFIER_STATE,             //4 - identifier or (reserved)keyword
    IS_STRING_STATE,                  //5 - string
    NUMBER_STATE,                     //6 - number
    FLOATING_STATE,                   //7 - number is floating point
    FLOAT_EXP_STATE,          //8 - floating point in exponent
    EXPONENT_STATE,                   //9 - exponent
    LOWER_STATE,                      //10 - lower operator - '<'
    HIGHER _STATE,                    //11 - higher operator - '>'
    ESCAPE_SEQUENCE_STATE,            //12 - escape sequence
    ESCAPE_NUMBER_STATE               //13 - \ddd number in escape sequence
} automata_state;

char* keywords[KEYWORDS] = {
    "as", "asc", "declare", "dim", "do", "double", "else", "end", "chr",
    "function", "if", "input", "integer", "length", "loop", "print", "return",
    "scope", "string", "substr", "then", "while",
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
    automata_states state = BEGIN_STATE;          //first state is BEGIN_STATE
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

/*********************************BEGIN_STATE STATE************************************/

        if (state == BEGIN_STATE) {
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
                    state = STRING_STATE;
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
                    state = MULTI_LINE_COMMENT_STATE;
                }
                else {
                    next = c;
                    next_token->type = SLASH_OP;
                    return OK;
                }
            }
            else if (isdigit(c)) {
                if (!stringAddChar(c, tmp_string)) {
                    state = NUMBER_STATE;
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
                state = LOWER_STATE;   
            }
            else if (c == '>') {
                state = HIGHER_STATE;
            }
            else if (c == '\n') {
                line++;
            }
            else if (isspace(c)) {
                //nothing to do, white characters are ignored
            }
            else if (c = EOF) {
                next_token->type = EOF_TOK;
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
                state = BEGIN_STATE;
            }
            else if (c == EOF) {
                next_token->type = EOF_TOK;
                return OK;
            }
            //we ignore comments
        }

/*********************************MULTI_LINE COMMENT STATE************************************/

        else if (state == MULTI_LINE_COMMENT_STATE) {
            if (c == APOSTROPHE) {
                if (c = getc(source_file) && c == '/') {
                    state = BEGIN_STATE;
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
                next_token->type = EOF_TOK;
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
                state = BEGIN_STATE;

                if ((int_tmp = identifierTest(tmp_string, keywords, reserved_keywords)) && int_tmp != -1) {
                    next_token->type = int_tmp;
                    return OK;
                }
                else {
                    next_token->type = IDENTIFIER_TOK;
                    return OK;
                }
            }
        }

/*********************************OPERATOR LOWER_STATE STATE************************************/

        else if (state == LOWER_STATE) {
            if (c == '>') {
                next_token->type = NO_EQUAL_OP;
                return OK;
            }
            else if (c == '=') {
                next_token->type = LOWER_STATE_EQUAL_OP;
                return OK;
            }
            else {
                next = c;
                next_token->type = LOWER_STATE_OP;
                return OK;
            }
        }

/*********************************OPERATOR HIGHER STATE************************************/

        else if (state == HIGHER_STATE) {
            if (c == '=') {
                next_token->type = HIGHER_STATE_EQUAL_OP;
                return OK;
            }
            else {
                next = c;
                next_token->type = HIGHER_STATE_OP;
                return OK;
            }
        }

/***********************************STRING STATE*******************************************/

        else if (state == STRING_STATE) {
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
                state = ESCAPE_SEQUENCE_STATE;
            }
            else if (c == EOF) {
                addError(line, LEX_ERROR);
                next_token->type = EOF_TOK;
                return LEX_ERROR;
            }
            else {
                addError(line, LEX_ERROR);
                next_token->type = STRING_TOK;
                return LEX_ERROR;
            }
        }

/*********************************NUMBER_STATE STATE************************************/

        else if (state == NUMBER_STATE) {
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
                    state = FLOAT_STATE_POINT_TOK;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (!stringAddChar(c, tmp_string)) {
                    state = EXPONENT_STATE;
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

/*********************************FLOAT_STATE_POINT STATE************************************/

        else if (state == FLOAT_STATE) {
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
                    state = EXPONENT_STATE;

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
                next_token->type = FLOAT_STATE_POINT_TOK;
                return OK;
            }
        }

/*********************************EXPONENT_STATE STATE************************************/

        else if (state == EXPONENT_STATE) {
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
                    state = FLOAT_EXP_STATE;
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
                next_token->type = FLOAT_STATE_POINT_TOK;
                return OK;
            }
        }

/*********************************FLOAT_EXP_STATE STATE************************************/

        else if (state == FLOAT_EXP_STATE) {
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
                next_token->type = FLOAT_STATE_POINT_TOK;
                return OK;
            }
        }

/*********************************ESCAPE_SEQUENCE_STATE STATE************************************/

        else if (state == ESCAPE_SEQUENCE_STATE) {
            if (c == QUOTE) {
                if (!stringAddChar(QUOTE, tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 'n') {
                if (!stringAddChar('\n', tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == 't') {
                if (!stringAddChar('\t', tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (c == BACKSLASH) {
                if (!stringAddChar(BACKSLASH, tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line, errors);
                    return MEM_ERROR;
                }
            }
            else if (isdigit(c)) {
                escape_number = c - 48;             //to get number from ascii table
                state = ESCAPE_NUMBER_STATE;
            }
            else {
                addError(line, LEX_ERROR);
                return LEX_ERROR;
            }
        }

/*********************************ESCAPE_NUMBER STATE************************************/

        else if (state == ESCAPE_NUMBER_STATE) {
            if (isdigit(c)) {
                escape_number = escape_number*10 + (c - 48);

                c = getc(source_file);
                if (isdigit(c)) {
                    escape_number = escape_number*10 + (c - 48);
                    if (!stringAddChar(escape_number, tmp_string)) {
                        state = STRING_STATE;
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
