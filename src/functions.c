#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "symtable.h"

bool call(char* name)
{
    // last_token.type is left bracket

    // Check symtable if function exists
    tSymbol* symbol = htSearch(var_table, name);
    if (!symbol)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Calling function '%s' that doesn't exist.", name);
    
    // Skip empty parameters
    if (symbol->arg_count == 0)
        UPDATE_LAST_TOKEN();

    for (int i = 0; i < symbol->arg_count; i++)
    {
        // TODO call expression evaluation
        // expression(symbol->arg_types[i]);

        // Check for colon between parameters
        if (i < symbol->arg_count - 1 && last_token.type != COLON_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected colon between parameters.");
    }

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected right parenthesis after function parameters.");

    printf("CALL $%s\n", name);

    return true;
}

void addParamToSymbol(tSymbol* symbol, char* name, token_type type)
{

}

bool function_params(tSymbol* symbol)
{
    // last_token.type is left bracket

    UPDATE_LAST_TOKEN();

    while (last_token.type == IDENTIFIER_TOK)
    {
        symbol->arg_count++;

        // Get parameter name
        char* var_name = last_token.attribute.string_ptr;

        UPDATE_LAST_TOKEN();

        // Check for AS after identifier
        if (last_token.type != AS)
            ERROR_AND_RETURN(SYN_ERROR, "Expected AS after identifier in function parameter.");

        UPDATE_LAST_TOKEN();

        switch (last_token.type)
        {
            case INTEGER:
            case DOUBLE:
            case STRING:
                // Unless is function declared
                if (symbol->type == UNDEFINED_TOK)
                    addParamToSymbol(symbol, var_name, last_token.type);
                else
                    ; // TODO check if param is same
                break;
            default:
                ERROR_AND_RETURN(SYN_ERROR, "Expected correct type of '%s' after AS.", var_name);
        }

        // FIXME wrong place
        printf("DEFVAR LF@%s\n", var_name);

        UPDATE_LAST_TOKEN();

        // Check for correct parameter ending
        if (last_token.type == COLON_OP)
            UPDATE_LAST_TOKEN();
        else if (last_token.type != RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected colon or right parenthesis after parameter.");
    }

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier in function parameter.");

    return true;
}

bool function_header(bool define)
{
    // last_token.type is FUNCTION

    UPDATE_LAST_TOKEN();

    // Check for identifier token
    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected function name after FUNCTION.");

    char* identif_name = last_token.attribute.string_ptr;

    // Check symtable if variable exists
    tSymbol* symbol = htSearch(func_table, last_token.attribute.string_ptr);
    
    // Handle multiple declaration
    if (!define && symbol)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Function '%s' is already declared.",
            last_token.attribute.string_ptr);
    
    // Handle multiple definitions
    if (define && symbol && symbol->defined)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Function '%s' is already defined.",
            last_token.attribute.string_ptr);

    UPDATE_LAST_TOKEN();

    // Check for left bracket after identifier
    if (last_token.type != LEFT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected left parenthesis after function name.");

    // Read params and return value
    if (!symbol)
    {
        symbol = malloc(sizeof(tSymbol));
        symbol->type = UNDEFINED_TOK;
        symbol->defined = false;
    }

    // read function params
    if (!function_params(symbol))
        return false;
    // last_token.type is right bracket

    UPDATE_LAST_TOKEN();

    // Check for AS after parameters
    if (last_token.type != AS)
        ERROR_AND_RETURN(SYN_ERROR, "Expected AS after function parameters.");

    UPDATE_LAST_TOKEN();

    // Read return type of function
    switch (last_token.type)
    {
        case INTEGER:
        case DOUBLE:
        case STRING:
            // Unless is function declared
            if (symbol->type == UNDEFINED_TOK)
                symbol->type = last_token.type;
            // Different return types at declaration and definition
            else if (symbol->type != last_token.type)
                ERROR_AND_RETURN(SEM_TYPE_ERROR,
                    "Different return types at declaration and definition.");
            break;
        default:
            ERROR_AND_RETURN(SYN_ERROR, "Expected correct return type after AS.");
    }

    // FIXME defined is still false
    if (!symbol->defined)
        htInsert(func_table, identif_name, *symbol);

    if (define)
    {
        printf("LABEL $%s\n", identif_name);
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
    }

    return true;
}

bool function_decl()
{
    // last_token.type is DECLARE

    UPDATE_LAST_TOKEN();
    
    if (last_token.type != FUNCTION)
        ERROR_AND_RETURN(SYN_ERROR, "Expected FUNCTION after DECLARE.");

    return function_header(false);
}

bool function_def()
{
    // last_token.type is FUNCTION

    // Parse function header (name, parameters and return value)
    if (!function_header(true))
        return false;
    // last_token.type is EOL_TOK

    // Parse the inside of function
    if (!statement_list())
        return false;

    // TODO clear var_table

    printf("POPFRAME\n");
    printf("RETURN\n");

    UPDATE_LAST_TOKEN();

    // Test the correct ending of function
    return last_token.type == FUNCTION;
}
