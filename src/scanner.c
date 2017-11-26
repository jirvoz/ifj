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

// States for finite automata
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
    // for extensions
    BASE_STATE                          //15 - binary, octal and hexadecimal numbers
} automata_state;

// Array of all keywords and reserved keywords
const char* keywords[KWD_COUNT] =
{
    "and", "as", "asc", "boolean", "chr", "continue", "declare", 
    "dim", "do", "double", "else", "elseif", "end", "exit", "false", 
    "for", "function", "if", "input", "integer", "length", "loop", 
    "next", "not", "or", "print", "return", "scope", "shared", "static", 
    "string", "substr", "then", "true", "while"
};

// Strings for errors in scanner
const char* memory_err = "Memory allocation problem";
const char* unknown_tok = "Unknown token";
const char* unexpected_tok = "Unexpected token";

// This function tests, if next token is operator(+,-,...)
int operatorTest(char c)
{
    switch (c)
    {
        case '+':   return PLUS_OP;
        case '-':   return MINUS_OP;
        case '*':   return STAR_OP;
        case '(':   return LEFT_PARENTH_OP;
        case ')':   return RIGHT_PARENTH_OP;
        case '=':   return EQUAL_SIGN_OP;
        case ',':   return COLON_OP;
        case ';':   return SEMICOLON_OP;
        case BACKSLASH: return BACKSLASH_OP;
        default:    return -1;
    }
}

// This function tests, if identifier is keyword
int identifierTest(string* identifier)
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

    // Identifier isn't keyword
    return -1;
}

// This function is called when error occured
bool returnFalse(err_code code, const char* message, string* str)
{
    stringFree(str);
    addError(code, message);
    return false;
}

// Main functions of scanner
int getNextToken (tToken* next_token, FILE* source_file)
{
    string tmp_string;
    stringInit(&tmp_string);

    automata_state state = BEGIN_STATE;     // First state is BEGIN_STATE
    int c;                      // Lexem
    int int_tmp = 10;           // Tmp int for identifiers and numbers BASE, default base is 10

    // Finite automata
    do
    {
        c = getc(source_file);                  // Get lexem from source file
        // IFJCODE17 is case insensitive, but strings aren't!
        if ((state != STRING_STATE) && (state != ESCAPE_SEQUENCE_STATE))
            c = tolower(c);

        // Main switch
        switch (state)
        {

/*********************************BEGIN STATE************************************/

            case BEGIN_STATE:
            {
                // Ignore white characters, but new line
                if (isspace(c))
                {
                    // New line token
                    if (c == '\n')
                    {
                        line++;
                        next_token->type = EOL_TOK;
                        FREE_AND_RETURN(tmp_string);
                    }
                    // Nothing to do, white characters are ignored
                }
                // Identifiers and keywords
                else if (isalpha(c) || c == '_')
                {
                    if (stringAddChar(c, &tmp_string))
                        state = IDENTIFIER_STATE;
                    else
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                }
                // Decimal numbers
                else if (isdigit(c))
                {
                    if (c == '0')
                    {
                        state = ZERO_STATE;     //ignore zeros  
                    }
                    else if (stringAddChar(c, &tmp_string))
                    {
                        state = NUMBER_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                }
                // Single operators 
                else if ((int_tmp = operatorTest(c)) != -1)
                {
                    next_token->type = int_tmp;
                    FREE_AND_RETURN(tmp_string);
                }

                // Other states depend on exact characters, we can use switch
                else
                {
                    switch (c)
                    {
                        // Bin, octa and hexa numbers
                        case '&':
                        {   
                            c = getc(source_file);
                            c = tolower(c);

                            switch(c)
                            {
                                case 'b': int_tmp = 2;
                                    break;
                                case 'o': int_tmp = 8;
                                    break;
                                case 'h': int_tmp = 16;
                                    break;
                                default: return returnFalse(LEX_ERROR, unknown_tok, &tmp_string);
                            }
                            state = ZERO_EXPONENT_STATE;    // Remove first zeros
                        }
                            break;
                        // String
                        case '!':
                        {
                            if ((c = getc(source_file)) == QUOTE)
                                state = STRING_STATE;
                            else
                                return returnFalse(LEX_ERROR, unknown_tok, &tmp_string);
                        }
                            break;
                        // Single line comment
                        case APOSTROPHE: state = SINGLE_LINE_COMMENT_STATE;
                            break; 
                        // Multi line comment or single operator /
                        case '/':
                        {
                            if ((c = getc(source_file)) == APOSTROPHE)
                                state = MULTI_LINE_COMMENT_STATE;
                            else
                            {
                                ungetc(c, source_file);
                                next_token->type = SLASH_OP;
                                FREE_AND_RETURN(tmp_string);
                            }
                        }
                            break;
                        // Multiple character operators
                        case '<': state = LOWER_STATE;  
                            break;
                        case '>': state = HIGHER_STATE;
                            break;
                        // End of file
                        case EOF:
                        {
                            next_token->type = EOF_TOK;
                            FREE_AND_RETURN(tmp_string);
                        }
                            break;
                        // Other character - error
                        default: return returnFalse(LEX_ERROR, unknown_tok, &tmp_string);
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
                    FREE_AND_RETURN(tmp_string);
                }
                else if (c == EOF)
                {
                    next_token->type = EOF_TOK;
                    FREE_AND_RETURN(tmp_string);
                }
                // We ignore comments
            }
                break;

/*********************************MULTI_LINE COMMENT STATE************************************/

            case MULTI_LINE_COMMENT_STATE:
            {
                if (c == APOSTROPHE)
                {
                    // End of multi line comment
                    if (getc(source_file) == '/')
                        state = BEGIN_STATE;
                }
                else if (c == '\n')
                {
                    line++;
                }
                else if (c == EOF)
                {
                    return returnFalse(LEX_ERROR, "Unexpected end of file in block comment", &tmp_string);
                }
                // We ignore comments
            }
                break;

/*********************************IDENTIFIER STATE************************************/

            case IDENTIFIER_STATE:
            {
                if (isalnum(c) || c == '_')
                {
                    if (!stringAddChar(c, &tmp_string))
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                }
                else
                {
                    ungetc(c, source_file);

                    if ((int_tmp = identifierTest(&tmp_string)) != -1)
                    {
                        next_token->type = int_tmp;
                        FREE_AND_RETURN(tmp_string);
                    }
                    else
                    {
                        next_token->type = IDENTIFIER_TOK;
                        next_token->attribute.string_ptr = tmp_string.str;
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
                    FREE_AND_RETURN(tmp_string);
                }
                else if (c == '=')
                {
                    next_token->type = LOWER_EQUAL_OP;
                    FREE_AND_RETURN(tmp_string);
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->type = LOWER_OP;
                    FREE_AND_RETURN(tmp_string);
                }
            }
                break;

/*********************************OPERATOR HIGHER STATE************************************/

            case HIGHER_STATE:
            {
                if (c == '=')
                {
                    next_token->type = HIGHER_EQUAL_OP;
                    FREE_AND_RETURN(tmp_string);
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->type = HIGHER_OP;
                    FREE_AND_RETURN(tmp_string);
                }
            }
                break;

/***********************************STRING STATE*******************************************/

            case STRING_STATE:
            {
                switch(c)
                {
                    case QUOTE:
                    {
                        next_token->type = STRING_TOK;
                        next_token->attribute.string_ptr = tmp_string.str;
                        return true;
                    }
                        break;
                    case ' ':
                    {
                        if (!stringConcat("\\032", &tmp_string))
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case '#':
                    {
                        if (!stringConcat("\\035", &tmp_string))
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case BACKSLASH:
                    {
                        if (!stringAddChar(c, &tmp_string))
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        state = ESCAPE_SEQUENCE_STATE;
                    }
                        break;
                    case EOF: return returnFalse(LEX_ERROR, "Unexpected end of file in string", &tmp_string);
                    default:
                    {
                        if (c > 31 && c <= 255)
                        {
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else
                            return returnFalse(LEX_ERROR, "Unknown character in string", &tmp_string);
                    }
                }
            }
                break;

/*********************************ESCAPE_SEQUENCE STATE************************************/

            case ESCAPE_SEQUENCE_STATE:
            {
                switch(c)
                {
                    case QUOTE:
                    {
                        if (stringConcat("034", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case 't':
                    {
                        if (stringConcat("009", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case 'n':
                    {
                        if (stringConcat("010", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case 'v':
                    {
                        if (stringConcat("011", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case 'f':
                    {
                        if (stringConcat("012", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case 'r':
                    {
                        if (stringConcat("013", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    case BACKSLASH:
                    {
                        if (stringConcat("092", &tmp_string))
                            state = STRING_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                        break;
                    default:
                    {
                        if (isdigit(c))
                        {
                            ungetc(c, source_file);
                            state = ESCAPE_NUMBER_STATE;
                        }
                        else
                            return returnFalse(LEX_ERROR, "Unknown escape sequence", &tmp_string);
                    }
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
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                    else
                        return returnFalse(LEX_ERROR, "Wrong escape sequence", &tmp_string);
                }
                
                long tmp = strtol(escape_string.str, NULL, 10);
    
                if (tmp > 0 && tmp < 256)
                {
                    stringConcat(escape_string.str, &tmp_string);
                    stringFree(&escape_string);
                    state = STRING_STATE;
                }
                else
                {
                    stringFree(&escape_string);
                    return returnFalse(LEX_ERROR, "Wrong escape sequence", &tmp_string);
                }
            }
                break;

/*********************************NUMBER STATE************************************/

            case NUMBER_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, &tmp_string))
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                }
                else if (c == '.')
                {
                    c = getc(source_file);

                    if (!isdigit(c))
                        return returnFalse(LEX_ERROR, "Wrong number", &tmp_string);
                    ungetc(c, source_file);

                    if (stringAddChar('.', &tmp_string))
                        state = FLOAT_STATE;
                    else
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringAddChar(c, &tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);

                        if (isdigit(c))
                            ungetc(c, source_file);
                        else if (c == '+' || c == '-')
                        {
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else
                            return returnFalse(LEX_ERROR, "Wrong number", &tmp_string);
                    }
                    else
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->type = INTEGER_TOK;
                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                    FREE_AND_RETURN(tmp_string);
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
                        // Ignore
                    }
                    else
                    {
                        if (stringAddChar(c, &tmp_string))
                            state = NUMBER_STATE;
                        else
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                }
                else if (c == 'e' || c == 'E')
                {
                    if (stringConcat("0e", &tmp_string))
                    {
                        state = EXPONENT_STATE;
                        c = getc(source_file);

                        if (c == '+' || c == '-')
                        {
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else if (!isdigit(c))
                        {
                            return returnFalse(LEX_ERROR, "Wrong number", &tmp_string);
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
                        return returnFalse(LEX_ERROR, "Wrong number", &tmp_string);
                    }
                    ungetc(c, source_file);

                    if (stringAddChar('.', &tmp_string))
                    {
                        state = FLOAT_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    if (!stringAddChar('0', &tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, "Memory allocation problem", &tmp_string);
                    }

                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                    next_token->type = INTEGER_TOK;
                    FREE_AND_RETURN(tmp_string);
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
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
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
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                            }
                        }
                        else if (!isdigit(c))
                        {
                            return returnFalse(LEX_ERROR, "Wrong number", &tmp_string);
                        }
                        else
                        {
                            ungetc(c, source_file);
                        }
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    next_token->type = FLOATING_POINT_TOK;
                    FREE_AND_RETURN(tmp_string);
                }
            }
                break;

/*********************************EXPONENT STATE************************************/

            case EXPONENT_STATE:
            {
                if (isdigit(c))
                {
                    if (!isdigit(tmp_string.str[tmp_string.length - 1]) && c == '0')
                    {
                        state = ZERO_EXPONENT_STATE;
                    }
                    else if (!stringAddChar(c, &tmp_string))
                    {
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    } 
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    next_token->type = FLOATING_POINT_TOK;
                    FREE_AND_RETURN(tmp_string);
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
                        if (stringAddChar(c, &tmp_string))
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
                            return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        } 
                    }
                }
                else if (c >= 'a' && c <= 'f' && int_tmp == 16)
                {
                    if (stringAddChar(c, &tmp_string))
                    {
                        state = BASE_STATE;
                    }
                    else
                    {
                        return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                    } 
                }
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    next_token->type = FLOATING_POINT_TOK;
                    FREE_AND_RETURN(tmp_string);
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
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else
                            base_ok = false;
                    }
                        break;
                    case 2:
                    {
                        if (c == '0' || c == '1')
                        {
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else
                            base_ok = false;    
                    }
                        break;
                    case 8:
                    {
                        if (c >= '0' && c <= '7')
                        {
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else
                            base_ok = false;
                    }
                        break;
                    case 16:
                    {
                        if (isdigit(c) || (c >= 'a' && c <= 'f'))
                        {
                            if (!stringAddChar(c, &tmp_string))
                                return returnFalse(OTHER_ERROR, memory_err, &tmp_string);
                        }
                        else
                            base_ok = false;
                    }
                        break;
                }

                if (!base_ok)
                {
                    ungetc(c, source_file);
                    next_token->type = INTEGER_TOK;
                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, int_tmp);
                    FREE_AND_RETURN(tmp_string);
                }
            }
                break;
            default: return returnFalse(OTHER_ERROR, "Unknown error", &tmp_string);
        }
    } while (1);
}
