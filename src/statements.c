#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "expressions.h"
#include "parser.h"
#include "scanner.h"
#include "statements.h"

bool dim_stat()
{
    // last_token.type is DIM

    UPDATE_LAST_TOKEN();

    if (last_token.type != IDENTIFIER_TOK)
        return false;

    char * identif_name = last_token.attribute.string_ptr;
    printf("DEFVAR LF@%s\n", identif_name);

    UPDATE_LAST_TOKEN();

    if (last_token.type != AS)
        return false;

    UPDATE_LAST_TOKEN();

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

    UPDATE_LAST_TOKEN();

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

    UPDATE_LAST_TOKEN();

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
        UPDATE_LAST_TOKEN();

        // TODO handle properly all types for printing
        printf("WRITE %s\n", last_token.attribute.string_ptr);

        UPDATE_LAST_TOKEN();

        if (last_token.type == EOL_TOK)
            return true;

        if (last_token.type != SEMICOLON_OP)
            return false;

    }
    while (true);
}
