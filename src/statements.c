#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "parser.h"
#include "scanner.h"

bool dim_stat()
{
    // last_token.type is DIM

    if (getNextToken(&last_token, stdin) != SUCCESS)
        return false;

    if (last_token.type != IDENTIFIER_TOK)
        return false;

    char * identif_name = last_token.attribute.string_ptr;
    printf("DEFVAR %s\n", identif_name);

    if (getNextToken(&last_token, stdin) != SUCCESS)
        return false;

    if (last_token.type != AS)
        return false;

    if (getNextToken(&last_token, stdin) != SUCCESS)
        return false;

    // TODO add to symbol table
    switch (last_token.type)
    {
        case INTEGER:
            break;
        case DOUBLE:
            break;
        case STRING:
            break;
        default:
            return false;
    }

    if (getNextToken(&last_token, stdin) != SUCCESS)
        return false;

    if (last_token.type == EOL_TOK)
        return true;
    else if (last_token.type != EQUAL_SIGN_OP)
        return false;

    // TODO value assignment
    return skip_statement();
}

bool assignment_stat()
{
    // last_token.type is IDENTIFIER_TOK

    if (getNextToken(&last_token, stdin) != SUCCESS)
        return false;

    if (last_token.type != EQUAL_SIGN_OP)
        return false;
    
    // TODO call expression parsing

    return skip_statement();
}

bool input_stat()
{
    // last_token.type is INPUT

    printf("WRITE string@?\\032");

    return skip_statement();
}

bool print_stat()
{
    // last_token.type is PRINT

    do
    {
        if (getNextToken(&last_token, stdin) != SUCCESS)
            return false;

        // TODO handle properly all types for printing
        printf("WRITE %s\n", last_token.attribute.string_ptr);

        if (getNextToken(&last_token, stdin) != SUCCESS)
            return false;

        if (last_token.type == EOL_TOK)
            return true;

        if (last_token.type != SEMICOLON_OP)
            return false;

    }
    while (true);
}
