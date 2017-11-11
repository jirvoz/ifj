#include <stdio.h>
#include "parser.h"
#include "errors.h"

tToken last_token;

bool statement()
{
    // TODO evaluate statements

    // skip to EOL or END token
    do
    {
        if (getNextToken(&last_token, stdin) != SUCCESS)
            return false;

        if (last_token.type == END)
            return true;

        if (last_token.type == EOL_TOK)
            return true;
    }
    while (true);
}

bool statement_list()
{
    // stop when hitted the end of block of code
    if (last_token.type == END)
        return true;

    // parse the actual statements
    return statement() && statement_list();
}

bool program()
{
    if (getNextToken(&last_token, stdin) != SUCCESS)
        return false;
    
    // TODO read functions

    // parse the main scope
    if (last_token.type == SCOPE)
    {
        printf("LABEL $$main\n");

        // parse the inside of scope
        if (!statement_list())
            return false;

        if (getNextToken(&last_token, stdin) != SUCCESS)
            return false;

        // test the correct ending of scope
        if (last_token.type == SCOPE)
            return true;
        else
            return false;
    }
    else
        return false;
}

// Main function that requests tokens and forges them to output code
bool parse()
{
    printf(".IFJcode17\n");
    printf("JUMP $$main\n");

    return program();

    // TODO make sure there isn't anything after "end scope"
}
