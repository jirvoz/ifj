#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        // Call expression evaluation
        expression(symbol->arg_types[i]);

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

bool addParamToSymbol(tSymbol* symbol, char* name, token_type type)
{
    if (symbol->arg_count + 1 >= symbol->arg_size)
    {
        symbol->arg_size += 8;
        symbol->arg_types = realloc(symbol->arg_types, symbol->arg_size * sizeof(token_type));
        // FIXME memory leak when arg_names lost
        symbol->arg_names = realloc(symbol->arg_names, symbol->arg_size * sizeof(char*));

        if (!symbol->arg_types || !symbol->arg_names)
            ERROR_AND_RETURN(OTHER_ERROR, "Memory allocation fail.");
    }

    symbol->arg_types[symbol->arg_count] = type;
    symbol->arg_names[symbol->arg_count] = name;

    symbol->arg_count++;

    return true;
}

bool function_params(tSymbol* symbol)
{
    // last_token.type is left bracket

    int param_count = 0;

    UPDATE_LAST_TOKEN();

    while (last_token.type == IDENTIFIER_TOK)
    {
        param_count++;

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
                // Unless is function declared add parameters to symbol in table
                if (symbol->type == UNDEFINED_TOK)
                {
                    if (!addParamToSymbol(symbol, var_name, last_token.type))
                        return false;
                }
                // Check if parameters are same
                else
                {
                    if (param_count > symbol->arg_count)
                        ERROR_AND_RETURN(SEM_PROG_ERROR, "Different parmeter count at definition.");
                    if (strcmp(var_name, symbol->arg_names[param_count]) != 0)
                        ERROR_AND_RETURN(SEM_PROG_ERROR, "Different parmeter name at definition.");
                    if (last_token.type != symbol->type)
                        ERROR_AND_RETURN(SEM_TYPE_ERROR, "Different parmeter type at definition.");
                }
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

    // Check same number of parameters at declaration and definition
    if (symbol->type == UNDEFINED_TOK && param_count != symbol->arg_count)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Different parmeter count at definition.");

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

        // Insert symbol to table of functions,
        // the pointer still points to same symbol in table
        htInsert(func_table, identif_name, *symbol);
    }

    // Read function parameters
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

    // Clear table of function variables
    htClearAll(var_table);

    // Test the correct ending of function block
    if (last_token.type != END)
        ERROR_AND_RETURN(SYN_ERROR, "Expected END at function ending.");

    printf("POPFRAME\n");
    printf("RETURN\n");

    UPDATE_LAST_TOKEN();

    // Test the correct ending of function
    return last_token.type == FUNCTION;
}
