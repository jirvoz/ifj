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
            printf("MOVE LF@%s int@0\n", identif_name);
            break;
        case DOUBLE:
            printf("MOVE LF@%s float@0\n", identif_name);
            break;
        case STRING:
            printf("MOVE LF@%s string@\n", identif_name);
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

    UPDATE_LAST_TOKEN();
    do
    {
        // TODO add printing of expressions

        switch (last_token.type)
        {
            case INTEGER_TOK:
                printf("WRITE int@%d\n", last_token.attribute.number);
                break;
            case FLOATING_POINT_TOK:
                printf("WRITE float@%f\n", last_token.attribute.float_number);
                break;
            case STRING_TOK:
                printf("WRITE string@%s\n", last_token.attribute.string_ptr);
                break;
            case IDENTIFIER_TOK:
                printf("WRITE LF@%s\n", last_token.attribute.string_ptr);
                break;
            default:
                return false;
        }

        // Read semicolon after expression
        UPDATE_LAST_TOKEN();

        if (last_token.type != SEMICOLON_OP)
            return false;

        // Read next expression or EOL after semicolon
        UPDATE_LAST_TOKEN();

        if (last_token.type == EOL_TOK)
            return true;

    }
    while (true);
}
