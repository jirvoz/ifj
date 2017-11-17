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
#include "errors.h"

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
    BIN_STATE,                          //15 - binary number
    OCTA_STATE,                         //16 - octal number
    HEXA_STATE                          //17 - hexadecimal number
} automata_state;

char* keywords[KWD_COUNT] =
{
    "and", "as", "asc", "boolean", "chr", "continue", "declare", 
    "dim", "do", "double", "else", "elseif", "end", "exit", "false", 
    "for", "function", "if", "input", "integer", "length", "loop", 
    "next", "not", "or", "print", "return", "scope", "shared", "static", 
    "string", "substr", "then", "true", "while"
};

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


int getNextToken (tToken* next_token, FILE* source_file)
{
    string tmp_string;
    stringInit(&tmp_string);
    next_token->attribute.string_ptr = tmp_string.str;

    automata_state state = BEGIN_STATE;         //first state is BEGIN_STATE
    int c;                                      //lexem
    int int_tmp;

    do
    {
        c = getc(source_file);                  //get lexem from source file
        if ((state != STRING_STATE) && (state != ESCAPE_SEQUENCE_STATE))
        {
            c = tolower(c);                     // IFJCODE17 is case insensitive
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
                    if (stringAddChar(c, &tmp_string))
                    {
                        state = IDENTIFIER_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }

                //decimal numbers
                else if (isdigit(c))
                {
                    if (c == '0')
                    {
                        state = ZERO_STATE;               //ignore zeros  
                    }
                    else if (stringAddChar(c, &tmp_string))
                    {
                        state = NUMBER_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }

                //single operators 
                else if ((int_tmp = operatorTest(c)) != -1)
                {
                    stringFree(&tmp_string);
                    next_token->type = int_tmp;
                    return true;
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
    
                                switch(c)
                                {
                                    case 'b': state = BIN_STATE;
                                        break;
                                    case 'o': state = OCTA_STATE;
                                        break;
                                    case 'h': state = HEXA_STATE;
                                        break;
                                    default: 
                                    {
                                        stringFree(&tmp_string);
                                        addError(LEX_ERROR, "Unknown token");
                                        return false;
                                    }
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
                                stringFree(&tmp_string);
                                ungetc(c, source_file);
                                addError(LEX_ERROR, "Unknown token");
                                return false;
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
                                stringFree(&tmp_string);
                                ungetc(c, source_file);
                                next_token->type = SLASH_OP;
                                return true;
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
                            stringFree(&tmp_string);
                            line++;
                            next_token->type = EOL_TOK;
                            return true;
                        }
                            break;
                        //end of file
                        case EOF:
                        {
                            stringFree(&tmp_string);
                            next_token->type = EOF_TOK;
                            return true; 
                        }
                            break;

                        //other character - error
                        default:
                        {
                            stringFree(&tmp_string);
                            addError(LEX_ERROR, "Unknown token");
                            return false;
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
                    stringFree(&tmp_string);
                    line++;
                    next_token->type = EOL_TOK;
                    return true;
                }
                else if (c == EOF)
                {
                    stringFree(&tmp_string);
                    next_token->type = EOF_TOK;
                    return true;
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
                    else
                    {
                        ungetc(c, source_file);
                    }
                }
                else if (c == '\n')
                {
                    line++;
                }
                else if (c == EOF)
                {
                    //unexpected
                    stringFree(&tmp_string);
                    addError(LEX_ERROR, NULL);
                    next_token->type = EOF_TOK;
                    return false;
                }
                //we ignore comments
            }
            break;

/*********************************IDENTIFIER STATE************************************/

            case IDENTIFIER_STATE:
            {
                if (isalnum(c) || c == '_')
                {
                    if (!stringAddChar(c, &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else
                {
                    ungetc(c, source_file);

                    if ((int_tmp = identifierTest(&tmp_string, keywords)) != -1)
                    {
                        stringFree(&tmp_string);
                        next_token->type = int_tmp;
                        return true;
                    }
                    else
                    {
                        next_token->attribute.string_ptr = tmp_string.str;
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
                    stringFree(&tmp_string);
                    next_token->type = NO_EQUAL_OP;
                    return true;
                }
                else if (c == '=')
                {
                    stringFree(&tmp_string);
                    next_token->type = LOWER_EQUAL_OP;
                    return true;
                }
                else
                {
                    stringFree(&tmp_string);
                    ungetc(c, source_file);
                    next_token->type = LOWER_OP;
                    return true;
                }
            }
            break;

/*********************************OPERATOR HIGHER STATE************************************/

            case HIGHER_STATE:
            {
                if (c == '=')
                {
                    stringFree(&tmp_string);
                    next_token->type = HIGHER_EQUAL_OP;
                    return true;
                }
                else
                {
                    stringFree(&tmp_string);
                    ungetc(c, source_file);
                    next_token->type = HIGHER_OP;
                    return true;
                }
            }
            break;

/***********************************STRING STATE*******************************************/

            case STRING_STATE:
            {
                if (c == QUOTE)
                {
                    next_token->attribute.string_ptr = tmp_string.str;
                    next_token->type = STRING_TOK;
                    return true;
                }
                else if (c == ' ')
                {
                    if (!stringConcat("\\032", &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == '#')
                {
                if (!stringConcat("\\035", &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == BACKSLASH)
                {
                    if (!stringAddChar(c, &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                    state = ESCAPE_SEQUENCE_STATE;
                }
                else if (c > 31 && c <= 255)
                {
                    if (!stringAddChar(c, &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == EOF)
                {
                    stringFree(&tmp_string);
                    addError(LEX_ERROR, NULL);
                    return false;
                }
                else
                {
                    stringFree(&tmp_string);
                    addError(LEX_ERROR, NULL);
                    return false;
                }
            }
            break;

/*********************************ESCAPE_SEQUENCE STATE************************************/

            case ESCAPE_SEQUENCE_STATE:
            {
                if (c == QUOTE)
                {
                    if (stringConcat("034", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 't')
                {
                    if (stringConcat("009", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 'n')
                {
                    if (stringConcat("010", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 'v')
                {
                    if (stringConcat("011", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 'f')
                {
                    if (stringConcat("012", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 'r')
                {
                    if (stringConcat("013", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == BACKSLASH)
                {
                    if (stringConcat("092", &tmp_string))
                    {
                        state = STRING_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (isdigit(c))
                {
                    ungetc(c, source_file);
                    state = ESCAPE_NUMBER_STATE;
                }
                else
                {
                    stringFree(&tmp_string);
                    addError(LEX_ERROR, NULL);
                    return false;
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
                            stringFree(&tmp_string);
                            addError(OTHER_ERROR, "Memory allocation problem");
                            return false;
                        }
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(LEX_ERROR, NULL);
                        return false;
                    }
                }
                
                long tmp;
                    tmp = strtol(escape_string.str, NULL, 10);
    
                if (tmp > 0 && tmp < 256)
                {
                    stringConcat(escape_string.str, &tmp_string);
                    stringFree(&escape_string);
                    state = STRING_STATE;
                }
                else
                {
                    stringFree(&tmp_string);
                    addError(LEX_ERROR, NULL);
                    return false;
                }
            }
            break;

/*********************************NUMBER STATE************************************/

            case NUMBER_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == '.')
                {
                    c = getc(source_file);

                    if (!isdigit(c))
                    {
                        stringFree(&tmp_string);
                        addError(LEX_ERROR, NULL);
                        return false;
                    }
                    ungetc(c, source_file);

                    if (stringAddChar('.', &tmp_string))
                    {
                        state = FLOAT_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringAddChar(c, &tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);

                        if ((c == '+' || c == '-'))
                        {
                            if (!stringAddChar(c, &tmp_string))
                            {
                                stringFree(&tmp_string);
                                addError(OTHER_ERROR, "Memory allocation problem");
                                return false;
                            }
                        }
                    else if (!isdigit(c))
                        {
                            stringFree(&tmp_string);
                            addError(LEX_ERROR, NULL);
                            return false;
                        }
                        else
                        {
                            ungetc(c, source_file);
                        }
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                    stringFree(&tmp_string);
                    next_token->type = INTEGER_TOK;
                    return true;
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
                        if (stringAddChar(c, &tmp_string))
                        {
                            state = NUMBER_STATE;
                        }
                        else
                        {
                            stringFree(&tmp_string);
                            addError(OTHER_ERROR, "Memory allocation problem");
                            return false;
                        } 
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringConcat("0e", &tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);

                        if ((c == '+' || c == '-'))
                        {
                            if (!stringAddChar(c, &tmp_string))
                            {
                                stringFree(&tmp_string);
                                addError(OTHER_ERROR, "Memory allocation problem");
                                return false;
                            }
                        }
                        else if (!isdigit(c))
                        {
                            stringFree(&tmp_string);
                            addError(LEX_ERROR, NULL);
                            return false;
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
                        stringFree(&tmp_string);
                        addError(LEX_ERROR, NULL);
                        return false;
                    }
                    ungetc(c, source_file);

                    if (stringAddChar('.', &tmp_string))
                    {
                        state = FLOAT_STATE;
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    if (!stringAddChar('0', &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }

                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                    stringFree(&tmp_string);
                    next_token->type = INTEGER_TOK;
                    return true;
                }
            }
            break;

/*********************************FLOAT STATE************************************/

            case FLOAT_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringAddChar(c, &tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);
    
                        if ((c == '+' || c == '-'))
                        {
                            if (!stringAddChar(c, &tmp_string))
                            {
                                stringFree(&tmp_string);
                                addError(OTHER_ERROR, "Memory allocation problem");
                                return false;
                            }
                        }
                        else if (!isdigit(c))
                        {
                            stringFree(&tmp_string);
                            addError(LEX_ERROR, NULL);
                            return false;
                        }
                        else
                        {
                            ungetc(c, source_file);
                        }
                    }
                    else
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    stringFree(&tmp_string);
                    next_token->type = FLOATING_POINT_TOK;
                    return true;
                }
            }
            break;

/*********************************EXPONENT STATE************************************/

            case EXPONENT_STATE:
            {
                if (isdigit(c))
                {
                    if (c == '0')
                    {
                        state = ZERO_EXPONENT_STATE;
                    }
                    else if (!stringAddChar(c, &tmp_string))
                    {
                        stringFree(&tmp_string);
                        addError(OTHER_ERROR, "Memory allocation problem");
                        return false;
                    } 
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    stringFree(&tmp_string);
                    next_token->type = FLOATING_POINT_TOK;
                    return true;
                }
            }
            break;

/*****************************ZERO_EXPONENT STATE********************************/

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
                        if (stringAddChar(c, &tmp_string))
                        {
                            state = EXPONENT_STATE;
                        }
                        else
                        {
                            stringFree(&tmp_string);
                            addError(OTHER_ERROR, "Memory allocation problem");
                            return false;
                        } 
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    stringFree(&tmp_string);
                    next_token->type = FLOATING_POINT_TOK;
                    return true;
                }
            }
            break;
        }
    } while (1);

}
