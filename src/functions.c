#include <stdio.h>
#include "errors.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"

bool call(char* name)
{
    return true;
}

bool function_decl()
{
    // last_token.type is DECLARE

    if (!skip_statement())
        return false;
    return true;
}

bool function_def()
{
    // last_token.type is FUNCTION

    UPDATE_LAST_TOKEN();
    
    if (last_token.type != IDENTIFIER_TOK)
        return false;
    
    printf("LABEL $%s\n", last_token.attribute.string_ptr);

    if (!skip_statement())
        return false;

    // parse the inside of function
    if (!statement_list())
        return false;

    UPDATE_LAST_TOKEN();

    // test the correct ending of function
    return last_token.type == FUNCTION;
}
