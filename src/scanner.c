//  Course:      Formal Languages and Compilers (IFJ)                         
//  Project:     Implementation of the IFJ17 imperative language compiler     
//  File:        scanner.c                                                  
//  Description: Source file of lexical analyser. 
//               Lexical analyser is implemented as a finite state automata.                                                       
//                                                                            
//  Authors: Tomáš Nereča : xnerec00                                          
//           Samuel Obuch : xobuch00                                          
//           Jiří Vozár   : xvozar04                                          
//           Ján Farský   : xfarsk00                                          

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

unsigned line = 1;      //line counter

// States for finite automata
typedef enum automata_state
{
    BEGIN_STATE,                        // Initial state
    SINGLE_LINE_COMMENT_STATE,          // Single line comment
    MULTI_LINE_COMMENT_STATE,           // Multi line comment
    IDENTIFIER_STATE,                   // Identifier or (reserved)keyword
    LOWER_STATE,                        // Lower operator - '<'
    HIGHER_STATE,                       // Higher operator - '>'
    STRING_STATE,                       // String
    ESCAPE_SEQUENCE_STATE,              // Escape sequence in string
    ESCAPE_NUMBER_STATE,                // \ddd number in escape sequence
    NUMBER_STATE,                       // Decimal number
    DOUBLE_STATE,                       // Floating point number
    EXPONENT_CHECK_STATE,               // Check if exponent isn't empty
    EXPONENT_STATE,                     // Exponential floating point number
    BASE_STATE                          // Binary, octal and hexadecimal numbers   
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

// Main functions of scanner
int getNextToken (tToken* next_token, FILE* source_file)
{
    // String to build token
    string tmp_string;
    stringInit(&tmp_string);

    automata_state state = BEGIN_STATE;     // First state is BEGIN_STATE
    int c;                                  // Lexem
    int int_tmp = 10;                       // Tmp int for identifiers and numbers BASE, default base is 10

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

/*********************************BEGIN_STATE************************************/

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
                        RETURN_TRUE(tmp_string);
                    }
                    // Nothing to do, white characters are ignored
                }
                // Identifiers and keywords
                else if (isalpha(c) || c == '_')
                {
                    if (stringAddChar(c, &tmp_string))
                        state = IDENTIFIER_STATE;
                    else
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Decimal numbers
                else if (isdigit(c))
                {
                    if (stringAddChar(c, &tmp_string))
                        state = NUMBER_STATE;
                    else
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Single operators 
                else if ((int_tmp = operatorTest(c)) != -1)
                {
                    next_token->type = int_tmp;
                    RETURN_TRUE(tmp_string);
                }

                // Other states depend on exact characters, we can use switch
                else
                {
                    switch (c)
                    {
                        // Bin, octa and hexa numbers
                        case '&':
                        {   
                            // Check if valid base number
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
                                default: RETURN_FALSE(tmp_string, LEX_ERROR, unknown_tok);
                            }
                            state = BASE_STATE;
                        }
                            break;
                        // String
                        case '!':
                        {
                            if ((c = getc(source_file)) == QUOTE)
                                state = STRING_STATE;
                            else
                                RETURN_FALSE(tmp_string, LEX_ERROR, unknown_tok);
                        }
                            break;
                        // Single line comment
                        case APOSTROPHE: state = SINGLE_LINE_COMMENT_STATE;
                            break; 
                        // Multi line comment or single operator /
                        case '/':
                        {
                            // If next character is apostrophe, / signalize multi line comment, 
                            // else single operator '/'
                            if ((c = getc(source_file)) == APOSTROPHE)
                                state = MULTI_LINE_COMMENT_STATE;
                            else
                            {
                                ungetc(c, source_file);
                                next_token->type = SLASH_OP;
                                RETURN_TRUE(tmp_string);
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
                            RETURN_TRUE(tmp_string);
                        }
                            break;
                        // Other character - error
                        default: RETURN_FALSE(tmp_string, LEX_ERROR, unknown_tok);
                    }
                }
            }
                break;

/*******************************SINGLE_LINE_COMMENT_STATE*********************************/

            case SINGLE_LINE_COMMENT_STATE:
            {
                // If new line, end of single comment
                if (c == '\n')
                {
                    line++;
                    next_token->type = EOL_TOK;
                    RETURN_TRUE(tmp_string);
                }
                // End of file
                else if (c == EOF)
                {
                    next_token->type = EOF_TOK;
                    RETURN_TRUE(tmp_string);
                }
                // We ignore comments
            }
                break;

/********************************MULTI_LINE_COMMENT_STATE*********************************/

            case MULTI_LINE_COMMENT_STATE:
            {
                if (c == APOSTROPHE)
                {
                    // Check if it's end of multi line comment
                    if (getc(source_file) == '/')
                        state = BEGIN_STATE;
                }
                else if (c == '\n')
                {
                    line++;         //counting line
                }
                // Unexpected end of file
                else if (c == EOF)
                {
                    RETURN_FALSE(tmp_string, LEX_ERROR, "Unexpected end of file in block comment");
                }
                // We ignore comments
            }
                break;

/*********************************IDENTIFIER_STATE**************************************/

            case IDENTIFIER_STATE:
            {
                // Valid identifier characters
                if (isalnum(c) || c == '_')
                {
                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                else
                {
                    ungetc(c, source_file);

                    // Test for keywords
                    if ((int_tmp = identifierTest(&tmp_string)) != -1)
                    {
                        next_token->type = int_tmp;
                        RETURN_TRUE(tmp_string);
                    }
                    // It's identifier
                    else
                    {
                        next_token->type = IDENTIFIER_TOK;
                        next_token->attribute.string_ptr = tmp_string.str;
                        return true;
                    }
                }
            }
                break;

/************************************LOWER_STATE*************************************/

            case LOWER_STATE:
            {
                switch(c)
                {   
                    // <> operator
                    case '>': next_token->type = NO_EQUAL_OP;
                        break;
                    // <= operator
                    case '=': next_token->type = LOWER_EQUAL_OP;
                        break;
                    // < operator
                    default: 
                        ungetc(c, source_file);
                        next_token->type = LOWER_OP;  
                }
                RETURN_TRUE(tmp_string);
            }
                break;

/*************************************HIGHER_STATE************************************/

            case HIGHER_STATE:
            {
                // >= operator
                if (c == '=')
                    next_token->type = HIGHER_EQUAL_OP;
                // > operator
                else
                {
                    ungetc(c, source_file);
                    next_token->type = HIGHER_OP;
                }
                RETURN_TRUE(tmp_string);
            }
                break;

/***********************************STRING_STATE*************************************/

            case STRING_STATE:
            {
                switch(c)
                {
                    // End of string
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
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case '#':
                    {
                        if (!stringConcat("\\035", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    // Escape Sequence
                    case BACKSLASH:
                    {
                        if (!stringAddChar(c, &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                        state = ESCAPE_SEQUENCE_STATE;
                    }
                        break;
                    // Unexpected end of file
                    case EOF: RETURN_FALSE(tmp_string, LEX_ERROR, "Unexpected end of file in string");
                    default:
                    {
                        if (c > 31 && c <= 255)
                        {
                            if (!stringAddChar(c, &tmp_string))
                                RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                        }
                        else
                            RETURN_FALSE(tmp_string, LEX_ERROR, "Unknown character in string");
                    }
                }
            }
                break;

/*******************************ESCAPE_SEQUENCE_STATE*********************************/

            case ESCAPE_SEQUENCE_STATE:
            {
                switch(c)
                {
                    case QUOTE:
                    {
                        if (!stringConcat("034", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case 't':
                    {
                        if (!stringConcat("009", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case 'n':
                    {
                        if (!stringConcat("010", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case 'v':
                    {
                        if (!stringConcat("011", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case 'f':
                    {
                        if (!stringConcat("012", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case 'r':
                    {
                        if (!stringConcat("013", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    case BACKSLASH:
                    {
                        if (!stringConcat("092", &tmp_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                        break;
                    default:
                    {
                        // Check if escape number
                        if (isdigit(c))
                        {
                            ungetc(c, source_file);
                            state = ESCAPE_NUMBER_STATE;
                        }
                        else
                            RETURN_FALSE(tmp_string, LEX_ERROR, "Unknown escape sequence");
                    }
                }
                // If no escape number, come back to string
                if (state != ESCAPE_NUMBER_STATE)
                    state = STRING_STATE;
            }
                break;

/*******************************ESCAPE_NUMBER_STATE*******************************/

            case ESCAPE_NUMBER_STATE:
            {
                // All numbers are taken in for cycle
                ungetc(c, source_file);
                // Tmp string for escape number
                string escape_string;
                stringInit(&escape_string);

                for (int i = 0; i < 3; i++)
                {
                    c = getc(source_file);
                    if (isdigit(c))
                    {
                        if (!stringAddChar(c, &escape_string))
                            RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                    }
                    else
                        RETURN_FALSE(tmp_string, LEX_ERROR, "Wrong escape sequence");
                }
                
                // Convert to number
                long tmp = strtol(escape_string.str, NULL, 10);
    
                // Check if valid escape number
                if (tmp > 0 && tmp < 256)
                {
                    stringConcat(escape_string.str, &tmp_string);
                    stringFree(&escape_string);
                    state = STRING_STATE;
                }
                else
                {
                    stringFree(&escape_string);
                    RETURN_FALSE(tmp_string, LEX_ERROR, "Wrong escape sequence");
                }
            }
                break;

/*********************************NUMBER_STATE************************************/

            case NUMBER_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Floating point number
                else if (c == '.')
                {
                    state = DOUBLE_STATE;
                    c = getc(source_file);

                    // Check if floating part isn't empty
                    if (!isdigit(c))
                        RETURN_FALSE(tmp_string, LEX_ERROR, "Wrong number");
                    ungetc(c, source_file);

                    if (!stringAddChar('.', &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Exponential floating point number
                else if (c == 'e' || c == 'E')
                {
                    // Check if exponent isn't
                    state = EXPONENT_CHECK_STATE;

                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Decimal number
                else
                {
                    ungetc(c, source_file);
                    next_token->type = INTEGER_TOK;
                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, 10);
                    RETURN_TRUE(tmp_string);
                }
            }
                break;

/*********************************DOUBLE_STATE************************************/

            case DOUBLE_STATE:
            {
                if (isdigit(c))
                {
                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                else if (c == 'e' || c == 'E')
                {
                    // Check if exponent isn't
                    state = EXPONENT_CHECK_STATE;

                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Non exponential floating point number
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    next_token->type = DOUBLE_TOK;
                    RETURN_TRUE(tmp_string);
                }
            }
                break;

/*********************************EXPONENT_CHECK_STATE******************************/

            case EXPONENT_CHECK_STATE:
            {
                // State is set, but not checked
                state = EXPONENT_STATE;

                // If number, OK
                if (isdigit(c))
                    ungetc(c, source_file);
                // Unnecessary sign
                else if (c == '+' || c == '-')
                {
                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);

                    // Check if next character is number
                    c = getc(source_file);

                    if (!isdigit(c))
                        RETURN_FALSE(tmp_string, LEX_ERROR, "Exponent can't be empty");

                    ungetc(c, source_file);
                }
                // Empty exponential part
                else
                    RETURN_FALSE(tmp_string, LEX_ERROR, "Exponent can't be empty");
            }
                break;

/*********************************EXPONENT_STATE************************************/

            case EXPONENT_STATE:
            {
                // Exponent - only numbers 
                if (isdigit(c))
                {
                    if (!stringAddChar(c, &tmp_string))
                        RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                }
                // Exponential flloating point
                else
                {
                    ungetc(c, source_file);
                    next_token->attribute.float_number = strtod(tmp_string.str, NULL);
                    next_token->type = DOUBLE_TOK;
                    RETURN_TRUE(tmp_string);
                }
            }
                break;

/**********************************BASE_STATE**************************************/

            case BASE_STATE:
            {
                // This flag signalize if it's end of number
                bool base_end = false;

                switch (int_tmp)
                {
                    // Binary number
                    case 2:
                    {
                        if (c == '0' || c == '1')
                        {
                            if (!stringAddChar(c, &tmp_string))
                                RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                        }
                        else
                            base_end = true;    
                    }
                        break;
                    // Octal number
                    case 8:
                    {
                        if (c >= '0' && c <= '7')
                        {
                            if (!stringAddChar(c, &tmp_string))
                                RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                        }
                        else
                            base_end = true;
                    }
                        break;
                    // Hexadecimal number
                    case 16:
                    {
                        if (isdigit(c) || (c >= 'a' && c <= 'f'))
                        {
                            if (!stringAddChar(c, &tmp_string))
                                RETURN_FALSE(tmp_string, OTHER_ERROR, memory_err);
                        }
                        else
                            base_end = true;
                    }
                        break;
                }
                // End of number
                if (base_end)
                {
                    ungetc(c, source_file);
                    next_token->type = INTEGER_TOK;
                    next_token->attribute.number = (int) strtol(tmp_string.str, NULL, int_tmp);
                    RETURN_TRUE(tmp_string);
                }
            }
                break;
            default: RETURN_FALSE(tmp_string, OTHER_ERROR, "Unknown error");
        }
    } while (1);
}
