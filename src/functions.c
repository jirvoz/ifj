//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        statements.c
//  Description: Source file of function parsing
//               Rules for parsing functions
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "symtable.h"

char* actual_function = NULL;

bool call(char* name)
{
    // last_token.type is left bracket

    // Check symtable if function exists
    tSymbol* symbol = htSearch(func_table, name);
    if (!symbol)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Calling function '%s' that doesn't exist.", name);
    
    // Skip empty parameters
    if (symbol->arg_count == 0)
        UPDATE_LAST_TOKEN();

    for (int i = 0; i < symbol->arg_count; i++)
    {
        UPDATE_LAST_TOKEN();
        if (last_token.type == RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SEM_TYPE_ERROR, "Expected more parameters for function '%s'.", name);
        // Call expression evaluation
        if(!expression(symbol->arg_types[i]))
            return false;

        // Check for colon between parameters
        if (i < symbol->arg_count - 1 && last_token.type != COLON_OP)
            ERROR_AND_RETURN(SEM_TYPE_ERROR, "Expected more parameters.");
    }

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SEM_TYPE_ERROR, "Expected right parenthesis after function parameters.");

    printf("CALL $%s\n", name);

    return true;
}

bool addParamToSymbol(tSymbol* symbol, char* name, token_type type)
{
    if (symbol->arg_count >= symbol->arg_size)
    {
        symbol->arg_size += 8;
        symbol->arg_types = realloc(symbol->arg_types, symbol->arg_size * sizeof(token_type));
        // FIXME memory leak when arg_names lost
        symbol->arg_names = realloc(symbol->arg_names, symbol->arg_size * sizeof(char*));

        if (!symbol->arg_types || !symbol->arg_names)
            ERROR_AND_RETURN(OTHER_ERROR, "Memory allocation fail.");
    }

    symbol->arg_types[symbol->arg_count] = type;

    // Allocate space for copy of function name including space for '\0'
    char* new_name = malloc((strlen(name) + 1) * sizeof(char));
    new_name = strcpy(new_name, name);

    symbol->arg_names[symbol->arg_count] = new_name;

    symbol->arg_count++;

    return true;
}

bool functionParams(tSymbol* symbol)
{
    // last_token.type is left bracket

    int param_count = 0;

    UPDATE_LAST_TOKEN();

    while (last_token.type == IDENTIFIER_TOK)
    {
        param_count++;

        // Get parameter name
        char* var_name = last_token.attribute.string_ptr;

        // Check for collision with functions name
        if (htSearch(func_table, var_name))
            ERROR_AND_RETURN(SEM_PROG_ERROR, "There is already function named '%s'.", var_name);

        // Check for same parameter names
        for (int i = 0; i < param_count - 1; i++)
            if (strcmp(var_name, symbol->arg_names[i]) == 0)
                ERROR_AND_RETURN(SEM_PROG_ERROR, "Duplicit name of function parameter.");

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
                    // Check parameter count
                    if (param_count > symbol->arg_count)
                        ERROR_AND_RETURN(SEM_TYPE_ERROR, "Higher parmeter count at definition.");
                    // Check same type of parameter
                    if (last_token.type != symbol->arg_types[param_count - 1])
                        ERROR_AND_RETURN(SEM_PROG_ERROR, "Different parmeter type at definition.");

                    // Update parameter name
                    free(symbol->arg_names[param_count - 1]);
                    char* new_name = malloc((strlen(var_name) + 1) * sizeof(char));
                    new_name = strcpy(new_name, var_name);
                    symbol->arg_names[param_count - 1] = new_name;
                }
                break;
            default:
                ERROR_AND_RETURN(SYN_ERROR, "Expected correct type of '%s' after AS.", var_name);
        }

        free(var_name);

        UPDATE_LAST_TOKEN();

        // Check for correct parameter ending
        if (last_token.type == COLON_OP)
            UPDATE_LAST_TOKEN();
        else if (last_token.type != RIGHT_PARENTH_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected colon or right parenthesis after parameter.");
    }

    // Check same number of parameters at declaration and definition
    if (symbol->type != UNDEFINED_TOK && param_count != symbol->arg_count)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Lower parmeter count at definition.");

    // Check for right bracket after parameters
    if (last_token.type != RIGHT_PARENTH_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier in function parameter.");

    return true;
}

bool functionHeader(bool define)
{
    // last_token.type is FUNCTION

    UPDATE_LAST_TOKEN();

    // Check for identifier token
    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected function name after FUNCTION.");

    char* identif_name = last_token.attribute.string_ptr;
    // Set global variable that we are in this function
    if (define)
        actual_function = identif_name;

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

    // Create new symbol if it doesn't exist
    if (!symbol)
    {
        tSymbol new_symbol;
        new_symbol.type = UNDEFINED_TOK;
        new_symbol.arg_count = 0;
        new_symbol.arg_size = 0;
        new_symbol.arg_types = NULL;
        new_symbol.arg_names = NULL;

        // Allocate space for copy of function name including space for '\0'
        char* new_name = malloc((strlen(identif_name) + 1) * sizeof(char));
        new_name = strcpy(new_name, identif_name);

        // Insert symbol to table of functions,
        // the pointer still points to same symbol in table
        htInsert(func_table, new_name, new_symbol);
        // Update pointer to symbol
        symbol = htSearch(func_table, last_token.attribute.string_ptr);
    }

    symbol->defined = define;
    if (actual_function != identif_name)
        free(identif_name);

    // Read function parameters
    if (!functionParams(symbol))
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
                ERROR_AND_RETURN(SEM_PROG_ERROR,
                    "Different return types at declaration and definition.");
            break;
        default:
            ERROR_AND_RETURN(SYN_ERROR, "Expected correct return type after AS.");
    }

    return true;
}

bool functionDecl()
{
    // last_token.type is DECLARE

    UPDATE_LAST_TOKEN();
    
    if (last_token.type != FUNCTION)
        ERROR_AND_RETURN(SYN_ERROR, "Expected FUNCTION after DECLARE.");

    return functionHeader(false);
}

bool functionDef()
{
    // last_token.type is FUNCTION

    // Parse function header (name, parameters and return value)
    if (!functionHeader(true))
        return false;
    // last_token.type is EOL_TOK

    // Get function data
    if (!actual_function)
        ERROR_AND_RETURN(OTHER_ERROR, "Can't get actual function name.");
    tSymbol* func_symbol = htSearch(func_table, actual_function);

    printf("LABEL $%s\n", actual_function);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare function parameters as variables
    for (int i = func_symbol->arg_count - 1; i >= 0; i--)
    {
        printf("DEFVAR LF@%s\n", func_symbol->arg_names[i]);

        switch (func_symbol->arg_types[i])
        {
            case INTEGER:
                printf("POPS LF@%s\n", func_symbol->arg_names[i]);
                break;
            case DOUBLE:
                printf("POPS LF@%s\n", func_symbol->arg_names[i]);
                break;
            case STRING:
                printf("POPS LF@%s\n", func_symbol->arg_names[i]);
                break;
            default:
                ERROR_AND_RETURN(OTHER_ERROR, "Bad type of function parameter.");
        }

        // Allocate space for copy of variable name including space for '\0'
        char* new_name = malloc((strlen(func_symbol->arg_names[i]) + 1) * sizeof(char));
        new_name = strcpy(new_name, func_symbol->arg_names[i]);

        htInsert(var_table, new_name,
            (tSymbol){ .type=func_symbol->arg_types[i], .defined = true, .arg_count = 0 });
    }

    // Parse the inside of function
    if (!statementList())
        return false;

    // Clear table of function variables
    htClear(var_table);
    free(actual_function);
    actual_function = NULL;

    // Test the correct ending of function block
    if (last_token.type != END)
        ERROR_AND_RETURN(SYN_ERROR, "Expected END at function ending.");

    switch (func_symbol->type)
    {
        case INTEGER:
            printf("PUSHS int@0\n");
            break;
        case DOUBLE:
            printf("PUSHS float@0\n");
            break;
        case STRING:
            printf("PUSHS string@\n");
            break;
        default:
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown function return type.");
    }
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    UPDATE_LAST_TOKEN();

    // Test the correct ending of function
    return last_token.type == FUNCTION;
}
