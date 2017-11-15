/* *******************************(IFJ 2017)********************************* */
/*  Course: Formal Languages and Compilers (IFJ) - FIT VUT Brno 2017/18       */
/*  Project: Implementation of the IFJ17 imperative language translator       */
/*                                                                            */
/*  Authors: Tomáš Nereča : xnerec00 : ()% (team leader)                      */
/*           Samuel Obuch : xobuch00 : ()%                                    */
/*           Jiří Vozár   : xvozar04 : ()%                                    */
/*           Ján Farský   : xfarsk00 : ()%                                    */
/* ************************************************************************** */

#include "../src/strings.h"
#include "../src/scanner.h"
#include <stdio.h>
#include <stdlib.h>

void TEST01(tToken* next_token)
{
    FILE* my_source = fopen("./tests/test1", "r");

    printf("[TEST01] Basic Test Int,Single Op and EOF\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Tokens:\n");

    while (1) {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else if (next_token->type == STRING_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else
        {
            printf("\n");
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    fclose(my_source);
}

void TEST02(tToken* next_token)
{
    FILE* my_source = fopen("./tests/test2", "r");

    printf("[TEST02] Basic Test KEY WORDs\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    printf("Tokens:\n");

    while (1)
    {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else if (next_token->type == STRING_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else
        {
            printf("\n");
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    fclose(my_source);
}

void TEST03(tToken* next_token)
{
    FILE* my_source=fopen("./tests/test3", "r");

    printf("[TEST03] Basic Test RESERVED KEY WORDs\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    printf("Tokens:\n");

    while (1)
    {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else if (next_token->type == STRING_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else
        {
            printf("\n");
        }

        if (next_token->type == EOF_TOK) {
            break;
        }  
    }

    fclose(my_source);
}

void TEST04(tToken* next_token)
{
    FILE* my_source=fopen("./tests/test4", "r");

    printf("[TEST04] Test correct identification \n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    printf("Tokens:\n");

    while (1)
    {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else if (next_token->type == STRING_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else
        {
            printf("\n");
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    fclose(my_source);
}

int main (int argc, char* argv[])
{
    tToken* next_token = malloc(sizeof (tToken));

    printf ("---------------------IFJ Scanner - Tests-----------------------\n");
    printf ("---------------------------------------------------------------\n\n");

    TEST01(next_token);
    TEST02(next_token);
    TEST03(next_token);
    TEST04(next_token);

    free(next_token);
    printf("\n----- Scanner - The End of Tests -----\n");

    return(0);
}
