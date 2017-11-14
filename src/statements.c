#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "expressions.h"
#include "parser.h"
#include "scanner.h"
#include "statements.h"
#include "symtable.h"

bool dim_stat()
{
    // last_token.type is DIM

    UPDATE_LAST_TOKEN();

    // Check for identifier token
    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier after DIM.");

    char * identif_name = last_token.attribute.string_ptr;

    // Check for collision with function name
    if (htSearch(func_table, identif_name))
        ERROR_AND_RETURN(SEM_PROG_ERROR, "There is already function named '%s'.", identif_name);

    // Check for redefinition
    if (htSearch(var_table, identif_name))
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Variable '%s' is already defined.", identif_name);

    printf("DEFVAR LF@%s\n", identif_name);

    UPDATE_LAST_TOKEN();

    // Check for AS token
    if (last_token.type != AS)
        ERROR_AND_RETURN(SYN_ERROR, "Expected AS after identifier.");

    UPDATE_LAST_TOKEN();

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

    htInsert(var_table, identif_name, (tSymbol){ .type=last_token.type });

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

    // Check symtable if variable exists
    tSymbol* symbol = htSearch(var_table, last_token.attribute.string_ptr);
    if (!symbol)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Undefined variable after INPUT.");

    // Write read instruction with first parameter
    printf("READ LF@%s ", last_token.attribute.string_ptr);

    // Write second parameter - type of input
    switch (symbol->type)
    {
        case INTEGER:
            printf("int\n");
            break;
        case DOUBLE:
            printf("float\n");
            break;
        case STRING:
            printf("string\n");
            break;
        case BOOLEAN:
            printf("bool\n");
            break;
        default:
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown type of variable at input.");
    }

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
