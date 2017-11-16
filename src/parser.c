#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "functions.h"
#include "parser.h"
#include "statements.h"

// Shared last-read token
tToken last_token;

// Symbol tables
tHtable* func_table;
tHtable* var_table;

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
            return if_stat();
            break; 
        case DO:
            return while_stat();
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
            ERROR_AND_RETURN(SYN_ERROR, "Wrong beginning of statement.");
    }

}

bool statement_list()
{
    switch (last_token.type)
    {
        case END:
        case ELSE:
        case LOOP:
            // stop when hitted the end of block of code
            return true;
        default:
            // parse the actual statements
            return statement() && statement_list();
    }
}

bool program()
{
    do 
    {
        UPDATE_LAST_TOKEN();
        
        // TODO read functions
        switch (last_token.type)
        {
            case DECLARE:
                if (!function_decl())
                    return false;
                break;
            case FUNCTION:
                if (!function_def())
                    return false;
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
                    ERROR_AND_RETURN(SYN_ERROR, "Bad ending of scope.");
                break;
            case EOL_TOK:
                // spare EOL is fine
                break;
            case EOF_TOK:
                ERROR_AND_RETURN(SYN_ERROR, "Missing main scope.");
            default:
                ERROR_AND_RETURN(SYN_ERROR, "Wrong beginning of statement.");
        }
    }
    while (true);
}

// Main function that requests tokens and forges them to output code
bool parse()
{
    printf(".IFJcode17\n");
    printf("JUMP $$main\n");

    func_table = malloc(sizeof(tHtitem) * HTSIZE);
    var_table = malloc(sizeof(tHtitem) * HTSIZE);
    htInit(func_table);
    htInit(var_table);

    if (!program())
        return false;

    // Make sure there isn't anything after "end scope"
    while (true)
    {
        UPDATE_LAST_TOKEN();
        switch (last_token.type)
        {
            case EOL_TOK:
                continue;
            case EOF_TOK:
                return true;
            default:
                ERROR_AND_RETURN(SYN_ERROR, "There is something after main scope.");
        }
    }

    htClearAll(func_table);
    htClearAll(var_table);
}
