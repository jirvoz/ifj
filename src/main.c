#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

int main ()
{
    token tok;
    do
    {
        if (getNextToken(&tok, stdin) != OK)
            return 1;

        switch (tok.type)
        {
            case IDENTIFIER:
            case STRING_TOK:
                printf("%d %s\n", tok.type, tok.attribute.identifier_string->str);
                break;
            case INTEGER_TOK:
                printf("%d %ld\n", tok.type, tok.attribute.number);
                break;
            default:
                printf("%d\n", tok.type);
                break;
        }
    }
    while (tok.type != END_OF_FILE);

    return 0;
}
