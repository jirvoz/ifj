#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "errors.h"

int main ()
{
    tToken tok;
    do
    {
        if (getNextToken(&tok, stdin) != SUCCESS)
            return 1;

        switch (tok.type)
        {
            case IDENTIFIER_TOK:
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
                free(tok.attribute.string_ptr);
                break;
        }
        
    }
    while (tok.type != EOF_TOK);

    return 0;
}