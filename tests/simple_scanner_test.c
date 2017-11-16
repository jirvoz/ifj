#include <stdio.h>
#include <stdlib.h>

#include "../src/scanner.h"
#include "../src/errors.h"

int main ()
{
    tToken tok;
    do
    {
        if (!getNextToken(&tok, stdin))
            return exit_code;

        switch (tok.type)
        {
            case IDENTIFIER_TOK:
                printf("%d %s\n", tok.type, tok.attribute.string_ptr);
                free(tok.attribute.string_ptr);
                break;
            case STRING_TOK:
                printf("%d %s\n", tok.type, tok.attribute.string_ptr);
                free(tok.attribute.string_ptr);
                break;
            case INTEGER_TOK:
                printf("%d %d\n", tok.type, tok.attribute.number);
                break;
            case FLOATING_POINT_TOK:
                printf("%d %f\n", tok.type, tok.attribute.float_number);
                break;
            default:
                printf("%d\n", tok.type);
        }
        
    }
    while (tok.type != EOF_TOK);

    return 0;
}