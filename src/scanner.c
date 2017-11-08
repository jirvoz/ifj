/* *******************************(IFJ 2017)********************************* */
/*  Course:  Formal Languages and Compilers (IFJ) - FIT VUT Brno 2017/18      */
/*  Project: Implementation of the IFJ17 imperative language translator       */
/*  File:    Source file of lexical analyser                                  */
/*                                                                            */
/*  Authors: Tomáš Nereča : xnerec00 : ()% (team leader)                      */
/*           Samuel Obuch : xobuch00 : ()%                                    */
/*           Jiří Vozár   : xvozar04 : ()%                                    */
/*           Ján Farský   : xfarsk00 : ()%                                    */
/* ************************************************************************** */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "errors.h"

unsigned line = 1;      //line counter

// states for finite automata

typedef enum automata_state {
    BEGIN_STATE,                        //0 - initial state
    SINGLE_LINE_COMMENT_STATE,          //1 - comment on 1 line
    MULTI_LINE_COMMENT_STATE,           //2 - multi line comment
    END_OF_COMMENT_STATE,               //3 - valid end of multi line comment
    IDENTIFIER_STATE,                   //4 - identifier or (reserved)keyword
    STRING_STATE,                       //5 - string
    NUMBER_STATE,                       //6 - number
    ZERO_STATE,
    ZERO_EXPONENT_STATE,
    FLOAT_STATE,                        //7 - number is floating point
    EXPONENT_STATE,                     //8 - exponent
    LOWER_STATE,                        //9 - lower operator - '<'
    HIGHER_STATE,                       //10 - higher operator - '>'
    ESCAPE_SEQUENCE_STATE,              //11 - escape sequence
    ESCAPE_NUMBER_STATE                 //12 - \ddd number in escape sequence
} automata_state;

char* keywords[KWD_COUNT] = {
    "as", "asc", "declare", "dim", "do", "double", "else", "end", "chr",
    "function", "if", "input", "integer", "length", "loop", "print", "return",
    "scope", "string", "substr", "then", "while",
    "and", "boolean", "continue", "elseif", "exit", "false", "for", "next",
    "not", "or", "shared", "static", "true"
};

int operatorTest(char c) {
    if (c == '+') {
        return PLUS_OP;
    }
    else if (c == '-') {
        return MINUS_OP;
    }
    else if (c == '*') {
        return STAR_OP;
    }
    else if (c == BACKSLASH) {
        return BACKSLASH_OP;
    }
    else if (c == '(') {
        return LEFT_PARENTH_OP;
    }
    else if (c == ')') {
        return RIGHT_PARENTH_OP;
    }
    else if (c == '=') {
        return EQUAL_SIGN_OP;
    }
    else if (c == ',') {
        return COLON_OP;
    }
    else if (c == ';') {
        return SEMICOLON_OP;
    }
    else {
        return -1;
    }
}

int getNextToken (tToken* next_token, FILE* source_file) {
    string tmp_string;
    stringInit(&tmp_string);
    next_token->attribute.string_ptr = tmp_string.str;

    automata_state state = BEGIN_STATE;          //first state is BEGIN_STATE
    int c;                                       //lexem
    int int_tmp;

   

    do {
        c = getc(source_file);                  //get lexem from source file
        if (isalpha(c) && isupper(c)) {
            c = tolower(c);                     // IFJCODE17 is case insensitive
        }

/*********************************BEGIN_STATE STATE************************************/

        if (state == BEGIN_STATE) {
            if (isalpha(c) || c == '_') {
                if (!(stringAddChar(c, &tmp_string))) {
                    state = IDENTIFIER_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == '!') {
                if ((c = getc(source_file)) == QUOTE) {
                    state = STRING_STATE;
                }
                else {
                    ungetc(c, source_file);
                    addError(line, FAILURE);
                    return FAILURE;
                }
            }
            else if (c == APOSTROPHE) {
                state = SINGLE_LINE_COMMENT_STATE;
            }
            else if (c == '/') {
                if ((c = getc(source_file)) == APOSTROPHE) {
                    state = MULTI_LINE_COMMENT_STATE;
                }
                else {
                    ungetc(c, source_file);
                    next_token->type = SLASH_OP;
                    return SUCCESS;
                }
            }
            else if (isdigit(c)) {
                if (c == '0') {
                    if (!stringAddChar(c, &tmp_string)) {
                        state = ZERO_STATE;               //ignore zeros 
                    }
                    else {
                        addError(line,OTHER_ERROR);
                        return FAILURE;
                    } 
                }
                else if (!(stringAddChar(c, &tmp_string))) {
                    state = NUMBER_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            //single operators 
            else if ((int_tmp = operatorTest(c)) && int_tmp != -1) {
                next_token->type = int_tmp;
                return SUCCESS;
            }
            else if (c == '<') {
                state = LOWER_STATE;   
            }
            else if (c == '>') {
                state = HIGHER_STATE;
            }
            else if (c == '\n') {
                line++;
                next_token->type = EOL_TOK;
                return SUCCESS;
            }
            else if (isspace(c)) {
                //nothing to do, white characters are ignored
            }
            else if (c == EOF) {
                next_token->type = EOF_TOK;
                return SUCCESS; 
            }
            else {
                addError(line, LEX_ERROR);
                return FAILURE;
            }
        }

/*********************************SINGLE_LINE COMMENT STATE************************************/

        else if (state == SINGLE_LINE_COMMENT_STATE) {
            if (c == '\n') {
                line++;
                state = BEGIN_STATE;
            }
            else if (c == EOF) {
                next_token->type = EOF_TOK;
                return SUCCESS;
            }
            //we ignore comments
        }

/*********************************MULTI_LINE COMMENT STATE************************************/

        else if (state == MULTI_LINE_COMMENT_STATE) {
            if (c == APOSTROPHE) {
                if ((c = getc(source_file)) == '/') {
                    state = BEGIN_STATE;
                }
                else {
                    ungetc(c, source_file);
                }
            }
            else if (c == '\n') {
                line++;
            }
            else if (c == EOF) {
                //unexpected
                addError(line, LEX_ERROR);
                next_token->type = EOF_TOK;
                return FAILURE;
            }
            //we ignore comments
        }

/*********************************IDENTIFIER STATE************************************/

        else if (state == IDENTIFIER_STATE) {
            if (isalnum(c) || c == '_') {
                if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else {
                ungetc(c, source_file);

                if ((int_tmp = identifierTest(&tmp_string, keywords)) != -1) {
                    next_token->type = int_tmp;
                    return SUCCESS;
                }
                else {
                    next_token->attribute.string_ptr = tmp_string.str;
                    next_token->type = IDENTIFIER_TOK;
                    return SUCCESS;
                }
            }
        }

/*********************************OPERATOR LOWER STATE************************************/

        else if (state == LOWER_STATE) {
            if (c == '>') {
                next_token->type = NO_EQUAL_OP;
                return SUCCESS;
            }
            else if (c == '=') {
                next_token->type = LOWER_EQUAL_OP;
                return SUCCESS;
            }
            else {
                ungetc(c, source_file);
                next_token->type = LOWER_OP;
                return SUCCESS;
            }
        }

/*********************************OPERATOR HIGHER STATE************************************/

        else if (state == HIGHER_STATE) {
            if (c == '=') {
                next_token->type = HIGHER_EQUAL_OP;
                return SUCCESS;
            }
            else {
                ungetc(c, source_file);
                next_token->type = HIGHER_OP;
                return SUCCESS;
            }
        }

/***********************************STRING STATE*******************************************/

        else if (state == STRING_STATE) {
            if (c == QUOTE) {
                next_token->attribute.string_ptr = tmp_string.str;
                next_token->type = STRING_TOK;
                return SUCCESS;
            }
            else if (c == ' ') {
                if (!stringConcat("\032", &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == '#') {
                if (!stringConcat("\035", &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c > 31 && c <= 255) {
                if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == BACKSLASH) {
                state = ESCAPE_SEQUENCE_STATE;
            }
            else if (c == EOF) {
                addError(line, LEX_ERROR);
                next_token->attribute.string_ptr = tmp_string.str;
                next_token->type = EOF_TOK;
                return FAILURE;
            }
            else {
                addError(line, LEX_ERROR);
                next_token->attribute.string_ptr = tmp_string.str;
                next_token->type = STRING_TOK;
                return FAILURE;
            }
        }

/*********************************NUMBER STATE************************************/

        else if (state == NUMBER_STATE) {
            if (isdigit(c)) {
                if (c == '0') {
                   //ignore zeros 
                }
                else if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == '.') {
                if (!stringAddChar(c, &tmp_string)) {
                    state = FLOAT_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (!stringAddChar(c, &tmp_string)) {
                    state = EXPONENT_STATE;

                    if ((c = getc(source_file)) && (c == '+' || c == '-')) {
                        if (!stringAddChar(c, &tmp_string)) {
                        }
                        else {
                            addError(line,OTHER_ERROR);
                            return FAILURE;
                        }
                    }
                    else {
                        ungetc(c, source_file);
                    }
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else {
                ungetc(c, source_file);
                next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                stringFree(&tmp_string);
                next_token->type = INTEGER_TOK;
                return SUCCESS;
            }
        }

/**********************************ZERO STATE************************************/

        else if (state == ZERO_STATE) {
            if (isdigit(c)) {
                if (c == '0') {
                //ignore
                }
                else {
                    stringClear(&tmp_string);

                    if (!stringAddChar(c, &tmp_string)) {
                        state = NUMBER_STATE;
                    }
                    else {
                        addError(line,OTHER_ERROR);
                        return FAILURE;
                    } 
                }
            }
            else if (c == 'e' || c == 'E') {
                if (!stringAddChar(c, &tmp_string)) {
                    state = EXPONENT_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == '.') {
                if (!stringAddChar(c, &tmp_string)) {
                    state = FLOAT_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else {
                ungetc(c, source_file);
                next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                stringFree(&tmp_string);
                next_token->type = INTEGER_TOK;
                return SUCCESS;
            }
        }

/*********************************FLOAT STATE************************************/

        else if (state == FLOAT_STATE) {
            if (isdigit(c)) {
                if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == 'e' || c == 'E') {
                if (!stringAddChar(c, &tmp_string)) {
                    state = EXPONENT_STATE;

                    if ((c = getc(source_file)) && (c == '+' || c == '-')) {
                        if (!stringAddChar(c, &tmp_string)) {
                        }
                        else {
                            addError(line,OTHER_ERROR);
                            return FAILURE;
                        }
                    }
                    else {
                        ungetc(c, source_file);
                    }
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else {
                ungetc(c, source_file);
                next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                stringFree(&tmp_string);
                next_token->type = FLOATING_POINT_TOK;
                return SUCCESS;
            }
        }

/*********************************EXPONENT STATE************************************/

        else if (state == EXPONENT_STATE) {
            if (isdigit(c)) {
                if (c == '0') {
                    state = ZERO_EXPONENT_STATE;
                }
                else if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                } 
            }
            else {
                ungetc(c, source_file);
                next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                stringFree(&tmp_string);
                next_token->type = FLOATING_POINT_TOK;
                return SUCCESS;
            }
        }

/*****************************ZERO_EXPONENT STATE********************************/

        else if (state == ZERO_EXPONENT_STATE) {
            if (isdigit(c)) {
                if (c == '0') {
                    //ignore
                }
                else {
                    if (!stringAddChar(c, &tmp_string)) {
                        state = EXPONENT_STATE;
                    }
                    else {
                        addError(line,OTHER_ERROR);
                        return FAILURE;
                    } 
                }
            }
            else {
                ungetc(c, source_file);
                next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                stringFree(&tmp_string);
                next_token->type = FLOATING_POINT_TOK;
                return SUCCESS;
            }
        }

/*********************************ESCAPE_SEQUENCE STATE************************************/

        else if (state == ESCAPE_SEQUENCE_STATE) {
            if (c == QUOTE) {
                if (!stringConcat("\034", &tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == 'n') {
                if (!stringConcat("\010", &tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == 't') {
                if (!stringConcat("\009", &tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (c == BACKSLASH) {
                if (!stringConcat("\092", &tmp_string)) {
                    state = STRING_STATE;
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else if (isdigit(c)) {
                state = ESCAPE_NUMBER_STATE;
                ungetc(c, source_file);
            }
            else {
                addError(line, LEX_ERROR);
                return FAILURE;
            }
        }

/*********************************ESCAPE_NUMBER STATE************************************/

        else if (state == ESCAPE_NUMBER_STATE) {
            if (c >= '0' && c <= '2') {
                if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else {
                addError(line, LEX_ERROR);
                return FAILURE;
            }

            c = getc(source_file);
            if (isdigit(c) && c >= '0' && c <= '5') {
                if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
            }
            else {
                addError(line, LEX_ERROR);
                return FAILURE;
            }

            c = getc(source_file);
            if (isdigit(c) && c >= '0' && c <= '5') {
                if (!stringAddChar(c, &tmp_string)) {
                }
                else {
                    addError(line,OTHER_ERROR);
                    return FAILURE;
                }
                state = STRING_STATE;
            }
            else {
                addError(line, LEX_ERROR);
                return FAILURE;
            }  
        }
    } while (1);

}
