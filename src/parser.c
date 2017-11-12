#include <stdio.h>
#include "errors.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"

tToken last_token;

// Temporary function to skip not implemented statements
bool skip_statement()
{
    do
    {
        UPDATE_LAST_TOKEN();

        if (last_token.type == END || last_token.type == EOL_TOK
            || last_token.type == EOF_TOK)
            return true;
    }
    while (true);
}

bool statement()
{
    UPDATE_LAST_TOKEN();
    // TODO evaluate statements
    switch (last_token.type)
    {
        case DIM:
            return dim_stat();
            break;
        case IDENTIFIER_TOK:
            return assignment_stat();
            // assignment
            break;
        case INPUT:
            return input_stat();
            break; 
        case PRINT:
            return print_stat();
            break; 
        case IF:
            return skip_statement();
            break; 
        case DO:
            return skip_statement();
            break; 
        case RETURN:
            return skip_statement();
            break; 
        case EOL_TOK:
        case END:
        case ELSE:
        case LOOP:
            return true;
            break;
        default:
            return false;
    }

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
    UPDATE_LAST_TOKEN();
    
    // TODO read functions
    switch (last_token.type)
    {
        case DECLARE:
            function_decl();
            break;
        case FUNCTION:
            function_def();
            break;
        case SCOPE:
            printf("LABEL $$main\n");
            printf("CREATEFRAME\n");
            printf("PUSHFRAME\n");

            // parse the inside of scope
            if (!statement_list())
                return false;

            UPDATE_LAST_TOKEN();

            // test the correct ending of scope
            if (last_token.type == SCOPE)
                return true;
            else
                return false;
            break;

        default:
            return false;
    }
    // empty program
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
