#include <stdio.h>
#include "errors.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"
#include "symtable.h"

bool call(char* name)
{
    // TODO search in symtable, evaluate params and call
    printf("CALL $%s\n", name);
    return true;
}

bool function_params(tSymbol symbol)
{
    return true;
}

bool function_header(bool define)
{
    // last_token.type is FUNCTION

    UPDATE_LAST_TOKEN();

    // Check for identifier token
    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier after DIM.");

    char * identif_name = last_token.attribute.string_ptr;

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

    printf("LABEL $%s\n", identif_name);
    
    // TODO: read params and return value

    if (!symbol)
        htInsert(func_table, identif_name, (tSymbol){ .type=UNDEFINED_TOK, .defined=define });

    if (!skip_statement())
        return false;

    printf("RETURN\n");
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

    // parse the inside of function
    if (!statement_list())
        return false;

    UPDATE_LAST_TOKEN();

    // test the correct ending of function
    return last_token.type == FUNCTION;
}
