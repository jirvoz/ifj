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
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier after DIM.");

    char * identif_name = last_token.attribute.string_ptr;
    printf("DEFVAR LF@%s\n", identif_name);

    UPDATE_LAST_TOKEN();

    if (last_token.type != AS)
        ERROR_AND_RETURN(SYN_ERROR, "Expected AS after identifier.");

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
            ERROR_AND_RETURN(SYN_ERROR, "Expected variable type after AS.");
    }

    UPDATE_LAST_TOKEN();

    if (last_token.type == EOL_TOK)
        return true;
    else if (last_token.type != EQUAL_SIGN_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected assignment symbol '=' ",
            "or end of line after declaration.");

    // TODO value assignment

    return skip_statement();
}

bool assignment_stat()
{
    // last_token.type is IDENTIFIER_TOK

    UPDATE_LAST_TOKEN();

    if (last_token.type != EQUAL_SIGN_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected assignment symbol '=' after identifier.");
        return false;
    
    // TODO call expression parsing

    return skip_statement();
}

bool input_stat()
{
    // last_token.type is INPUT

    printf("WRITE string@?\\032\n");

    // Read variable to use for input
    UPDATE_LAST_TOKEN();

    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier after INPUT.");

    // TODO check symtable and write code

    // EOL after input statement
    UPDATE_LAST_TOKEN();

    if (last_token.type == EOL_TOK)
        return true;
    else
        ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after variable for input.");
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
                ERROR_AND_RETURN(SYN_ERROR, "Can print only constants and single variables.");
        }

        // Read semicolon after expression
        UPDATE_LAST_TOKEN();

        if (last_token.type != SEMICOLON_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected semicolon after print value.");

        // Read next expression or EOL after semicolon
        UPDATE_LAST_TOKEN();

        if (last_token.type == EOL_TOK)
            return true;

    }
    while (true);
}
