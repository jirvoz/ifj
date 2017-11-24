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
#include <string.h>
#include "scanner.h"

unsigned line = 1;      //line counter

// states for finite automata
typedef enum automata_state
{
    BEGIN_STATE,                        //0 - initial state
    SINGLE_LINE_COMMENT_STATE,          //1 - comment on 1 line
    MULTI_LINE_COMMENT_STATE,           //2 - multi line comment
    END_OF_COMMENT_STATE,               //3 - valid end of multi line comment
    IDENTIFIER_STATE,                   //4 - identifier or (reserved)keyword
    STRING_STATE,                       //5 - string
    NUMBER_STATE,                       //6 - number
    ZERO_STATE,                         //7 - first zeros in number clean
    ZERO_EXPONENT_STATE,                //8 - first zeros in exponent clean
    FLOAT_STATE,                        //9 - number is floating point
    EXPONENT_STATE,                     //10 - exponent
    LOWER_STATE,                        //11 - lower operator - '<'
    HIGHER_STATE,                       //12 - higher operator - '>'
    ESCAPE_SEQUENCE_STATE,              //13 - escape sequence
    ESCAPE_NUMBER_STATE,                //14 - \ddd number in escape sequence
    //extensions
    BASE_STATE                          //15 - binary, octal and hexadecimal numbers
} automata_state;

char* keywords[KWD_COUNT] =
{
    "and", "as", "asc", "boolean", "chr", "continue", "declare", 
    "dim", "do", "double", "else", "elseif", "end", "exit", "false", 
    "for", "function", "if", "input", "integer", "length", "loop", 
    "next", "not", "or", "print", "return", "scope", "shared", "static", 
    "string", "substr", "then", "true", "while"
};

const char* memory_err = "Memory allocation problem";
const char* unknown_tok = "Unknown token";
const char* unexpected_tok = "Unexpected token";

//this functions tests, if next token is operator(+,-,...)
int operatorTest(char c)
{
    switch (c)
    {
        case '+':   return PLUS_OP;
            break;
        case '-':   return MINUS_OP;
            break;
        case '*':   return STAR_OP;
            break;
        case '(':   return LEFT_PARENTH_OP;
            break;
        case ')':   return RIGHT_PARENTH_OP;
            break;
        case '=':   return EQUAL_SIGN_OP;
            break;
        case ',':   return COLON_OP;
            break;
        case ';':   return SEMICOLON_OP;
            break;
        case BACKSLASH: return BACKSLASH_OP;
            break;
        default:    return -1;
    }
}

//this functions tests, if identifier is keyword
int identifierTest(string* identifier, char** keywords)
{
    int left = 0;
    int right = KWD_COUNT - 1;
    int middle;

    do 
    {
        middle = (left + right) / 2;

        if (strcmp(identifier->str, keywords[middle]) < 0)
        {
            right = middle - 1;
        }
        else if (strcmp(identifier->str, keywords[middle]) > 0)
        {
            left = middle + 1;
        }
        else
        {
            return (middle + 30);
        }
    } while (right >= left);

    return -1;
}

bool returnClean(string* str)
{
    free(str);
    return true;
}

//this functions is called is error occured
bool returnFalse(err_code code, const char* message, string* str)
{
    stringFree(str);
    free(str);
    addError(code, message);
    return false;
}

int getNextToken (tToken* next_token, FILE* source_file)
{
    string* tmp_string = malloc(sizeof(string));
    stringInit(tmp_string);

    automata_state state = BEGIN_STATE;         //first state is BEGIN_STATE
    int c;                                      //lexem
    int int_tmp = 10;                           //tmp int for identifiers and numbers BASE

    do
    {
        c = getc(source_file);                  //get lexem from source file

        // IFJCODE17 is case insensitive
        if ((state != STRING_STATE) && (state != ESCAPE_SEQUENCE_STATE))
        {
            c = tolower(c);
        }

/*********************************BEGIN STATE************************************/

        switch (state)
        {
            case BEGIN_STATE:
            {
                //ignore white characters, but new line
                if (isspace(c) && (c != '\n'))
                {
                    //nothing to do, white characters are ignored
                }

                //identifiers and keywords
                else if (isalpha(c) || c == '_')
                {
                    if (stringAddChar(c, tmp_string))
                    {
                        state = IDENTIFIER_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }

                //decimal numbers
                else if (isdigit(c))
                {
                    if (c == '0')
                    {
                        state = ZERO_STATE;               //ignore zeros  
                    }
                    else if (stringAddChar(c, tmp_string))
                    {
                        state = NUMBER_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }

                //single operators 
                else if ((int_tmp = operatorTest(c)) != -1)
                {
                    stringFree(tmp_string);
                    next_token->type = int_tmp;
                    return returnClean(tmp_string);
                }

                //Other states depend on exact characters, we can use switch
                else
                {
                    switch (c)
                    {
                        //bin, octa and hexa numbers
                        case '&':
                        {   
                            c = getc(source_file);
                            c = tolower(c);
                            state = ZERO_EXPONENT_STATE;    //remove first zeros
    
                                switch(c)
                                {
                                    case 'b': int_tmp = 2;
                                        break;
                                    case 'o': int_tmp = 8;
                                        break;
                                    case 'h': int_tmp = 16;
                                        break;
                                    default: return returnFalse(LEX_ERROR, unknown_tok, tmp_string);
                                }
                        }
                            break;

                        //string
                        case '!':
                        {
                            if ((c = getc(source_file)) == QUOTE)
                            {
                                state = STRING_STATE;
                            }
                            else
                            {
                                return returnFalse(LEX_ERROR, unknown_tok, tmp_string);
                            }
                        }
                            break;

                        //single line comment
                        case APOSTROPHE:
                        {
                            state = SINGLE_LINE_COMMENT_STATE;
                        }
                            break; 

                        //multi line comment
                        case '/':
                        {
                            if ((c = getc(source_file)) == APOSTROPHE)
                            {
                                state = MULTI_LINE_COMMENT_STATE;
                            }
                            else
                            {
                                stringFree(tmp_string);
                                ungetc(c, source_file);
                                next_token->type = SLASH_OP;
                                return returnClean(tmp_string);
                            }
                        }
                            break;

                        //multiple charakter operators
                        case '<':
                        {
                            state = LOWER_STATE;   
                        }
                            break;
                        case '>':
                        {
                            state = HIGHER_STATE;
                        }
                            break;

                        //new line token
                        case '\n':
                        {
                            line++;
                            next_token->type = EOL_TOK;
                            stringFree(tmp_string);
                            return returnClean(tmp_string);
                        }
                            break;

                        //end of file
                        case EOF:
                        {
                            next_token->type = EOF_TOK;
                            stringFree(tmp_string);
                            return returnClean(tmp_string);
                        }
                            break;

                        //other character - error
                        default:
                        {
                            return returnFalse(LEX_ERROR, unknown_tok, tmp_string);
                        }
                    }
                }
            }
                break;

/*********************************SINGLE_LINE COMMENT STATE************************************/

            case SINGLE_LINE_COMMENT_STATE:
            {
                if (c == '\n')
                {
                    line++;
                    next_token->type = EOL_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
                else if (c == EOF)
                {
                    next_token->type = EOF_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
                //we ignore comments
            }
                break;

/*********************************MULTI_LINE COMMENT STATE************************************/

            case MULTI_LINE_COMMENT_STATE:
            {
                if (c == APOSTROPHE)
                {
                    if ((c = getc(source_file)) == '/')
                    {
                        state = BEGIN_STATE;
                    }
                }
                else if (c == '\n')
                {
                    line++;
                }
                else if (c == EOF)
                {
                    //unexpected
                    return returnFalse(LEX_ERROR, "Unexpected end of file in block comment", tmp_string);
                }
                //we ignore comments
            }
                break;

/*********************************IDENTIFIER STATE************************************/

            case IDENTIFIER_STATE:
            {
                if (isalnum(c) || c == '_')
                {
                    if (!stringAddChar(c, tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else
                {
                    ungetc(c, source_file);

                    if ((int_tmp = identifierTest(tmp_string, keywords)) != -1)
                    {
                        next_token->type = int_tmp;
                        stringFree(tmp_string);
                        return returnClean(tmp_string);
                    }
                    else
                    {
                        next_token->attribute.string_ptr = tmp_string->str;
                        next_token->type = IDENTIFIER_TOK;
                        return true;
                    }
                }
            }
                break;

/*********************************OPERATOR LOWER STATE************************************/

            case LOWER_STATE:
            {
                if (c == '>')
                {
                    next_token->type = NO_EQUAL_OP;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
                else if (c == '=')
                {
                    next_token->type = LOWER_EQUAL_OP;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->type = LOWER_OP;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/*********************************OPERATOR HIGHER STATE************************************/

            case HIGHER_STATE:
            {
                if (c == '=')
                {
                    next_token->type = HIGHER_EQUAL_OP;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->type = HIGHER_OP;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/***********************************STRING STATE*******************************************/

            case STRING_STATE:
            {
                if (c == QUOTE)
                {
                    next_token->attribute.string_ptr = tmp_string->str;
                    next_token->type = STRING_TOK;
                    return true;
                }
                else if (c == ' ')
                {
                    if (!stringConcat("\\032", tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == '#')
                {
                if (!stringConcat("\\035", tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == BACKSLASH)
                {
                    if (!stringAddChar(c, tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                    state = ESCAPE_SEQUENCE_STATE;
                }
                else if (c > 31 && c <= 255)
                {
                    if (!stringAddChar(c, tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == EOF)
                {
                    return returnFalse(LEX_ERROR, "Unexpected end of file in string", tmp_string);
                }
                else
                {
                    return returnFalse(LEX_ERROR, "Unknown character in string", tmp_string);
                }
            }
                break;

/*********************************ESCAPE_SEQUENCE STATE************************************/

            case ESCAPE_SEQUENCE_STATE:
            {
                if (c == QUOTE)
                {
                    if (stringConcat("034", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 't')
                {
                    if (stringConcat("009", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 'n')
                {
                    if (stringConcat("010", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 'v')
                {
                    if (stringConcat("011", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 'f')
                {
                    if (stringConcat("012", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 'r')
                {
                    if (stringConcat("013", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == BACKSLASH)
                {
                    if (stringConcat("092", tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (isdigit(c))
                {
                    ungetc(c, source_file);
                    state = ESCAPE_NUMBER_STATE;
                }
                else
                {
                    return returnFalse(LEX_ERROR, "Unknown escape sequence", tmp_string);
                }
            }
                break;

/*********************************ESCAPE_NUMBER STATE************************************/

            case ESCAPE_NUMBER_STATE:
            {
                ungetc(c, source_file);
                string escape_string;
                stringInit(&escape_string);

                for (int i = 0; i < 3; i++)
                {
                    c = getc(source_file);
                    if (isdigit(c))
                    {
                        if (!stringAddChar(c, &escape_string))
                        {
                            return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                        }
                    }
                    else
                    {
                        return returnFalse(LEX_ERROR, "Wrong escape sequence", tmp_string);
                    }
                }
                
                long tmp = strtol(escape_string.str, NULL, 10);
    
                if (tmp > 0 && tmp < 256)
                {
                    stringConcat(escape_string.str, tmp_string);
                    stringFree(&escape_string);
                    state = STRING_STATE;
                }
                else
                {
                    stringFree(&escape_string);
                    return returnFalse(LEX_ERROR, "Wrong escape sequence", tmp_string);
                }
            }
                break;

/*********************************NUMBER STATE************************************/

            case NUMBER_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == '.')
                {
                    c = getc(source_file);

                    if (!isdigit(c))
                    {
                        return returnFalse(LEX_ERROR, "Wrong number", tmp_string);
                    }
                    ungetc(c, source_file);

                    if (stringAddChar('.', tmp_string))
                    {
                        state = FLOAT_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringAddChar(c, tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);

                        if ((c == '+' || c == '-'))
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else if (!isdigit(c))
                        {
                            return returnFalse(LEX_ERROR, "Wrong number", tmp_string);
                        }
                        else
                        {
                            ungetc(c, source_file);
                        }
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.number = (int) strtol(tmp_string->str, NULL, 10);
                    next_token->type = INTEGER_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/**********************************ZERO STATE************************************/

            case ZERO_STATE:
            {
                if (isdigit(c))
                {
                    if (c == '0')
                    {
                    //ignore
                    }
                    else
                    {
                        if (stringAddChar(c, tmp_string))
                        {
                            state = NUMBER_STATE;
                        }
                        else
                        {
                            return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                        } 
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringConcat("0e", tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);

                        if ((c == '+' || c == '-'))
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else if (!isdigit(c))
                        {
                            return returnFalse(LEX_ERROR, "Wrong number", tmp_string);
                        }
                        else
                        {
                            ungetc(c, source_file);
                        }
                    }
                }
                else if (c == '.')
                {
                    c = getc(source_file);

                    if (!isdigit(c))
                    {
                        return returnFalse(LEX_ERROR, "Wrong number", tmp_string);
                    }
                    ungetc(c, source_file);

                    if (stringAddChar('.', tmp_string))
                    {
                        state = FLOAT_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    if (!stringAddChar('0', tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, "Memory allocation problem", tmp_string);
                    }

                    next_token->attribute.number = (int) strtol(tmp_string->str, NULL, 10);
                    next_token->type = INTEGER_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/*********************************FLOAT STATE************************************/

            case FLOAT_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringAddChar(c, tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);
    
                        if ((c == '+' || c == '-'))
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else if (!isdigit(c))
                        {
                            return returnFalse(LEX_ERROR, "Wrong number", tmp_string);
                        }
                        else
                        {
                            ungetc(c, source_file);
                        }
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string->str, NULL);
                    next_token->type = FLOATING_POINT_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/*********************************EXPONENT STATE************************************/

            case EXPONENT_STATE:
            {
                if (isdigit(c))
                {
                    if (!isdigit(tmp_string->str[tmp_string->length - 1]) && c == '0')
                    {
                        state = ZERO_EXPONENT_STATE;
                    }
                    else if (!stringAddChar(c, tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    } 
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string->str, NULL);
                    next_token->type = FLOATING_POINT_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/*****************************ZERO_EXPONENT STATE********************************/

            //used also for remove zeros in "other base" number
            case ZERO_EXPONENT_STATE:
            {
                if (isdigit(c))
                {
                    if (c == '0')
                    {
                        //ignore
                    }
                    else
                    {
                        if (stringAddChar(c, tmp_string))
                        {
                            if (int_tmp == 10)
                            {   
                                state = EXPONENT_STATE;
                            }
                            else
                            {
                                state = BASE_STATE;
                            }
                        }
                        else
                        {
                            return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                        } 
                    }
                }
                else if (c >= 'a' && c <= 'f' && int_tmp == 16)
                {
                    if (stringAddChar(c, tmp_string))
                    {
                        state = BASE_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                    } 
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string->str, NULL);
                    next_token->type = FLOATING_POINT_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;

/*****************************BASE_STATE********************************/

            case BASE_STATE:
            {
                bool base_ok = true;
                switch (int_tmp)
                {
                    case 10:
                    {
                        if (isdigit(c))
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else
                            base_ok = false;
                    }
                        break;
                    case 2:
                    {
                        if (c == '0' || c == '1')
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else
                            base_ok = false;    
                    }
                        break;
                    case 8:
                    {
                        if (c >= '0' && c <= '7')
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else
                            base_ok = false;
                    }
                        break;
                    case 16:
                    {
                        if (isdigit(c) || (c >= 'a' && c <= 'f'))
                        {
                            if (!stringAddChar(c, tmp_string))
                            {
                                return returnFalse(OTHER_ERROR, memory_err, tmp_string);
                            }
                        }
                        else
                            base_ok = false;
                    }
                        break;
                }

                if (!base_ok)
                {
                    ungetc(c, source_file);
                    next_token->attribute.number = (int) strtol(tmp_string->str, NULL, int_tmp);
                    next_token->type = INTEGER_TOK;
                    stringFree(tmp_string);
                    return returnClean(tmp_string);
                }
            }
                break;
            default: return returnFalse(OTHER_ERROR, "Unknown error", tmp_string);
        }
    } while (1);
}
