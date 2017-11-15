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

void TEST01(FILE* my_source, tToken* next_token) {
    my_source = fopen("test1", "r");

    printf("[TEST01] Basic Test Int,Single Op and EOF\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Tokens:\n");

    while (1) {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    printf("Expected tokens:\n 2\n 16\n 2\n 12\n 2\n 5\n\n");

    fclose(my_source);
}

void TEST02(FILE* my_source, tToken* next_token) {
    my_source=fopen("test2", "r");

    printf("[TEST02] Basic Test KEY WORDs\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    printf("Tokens:\n");

    while (1) {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    printf("Expected tokens:\n 2\n 16\n 2\n 12\n 2\n 5\n\n");

    fclose(my_source);
}

void TEST03(FILE* my_source, tToken* next_token) {
    my_source=fopen("test3", "r");

    printf("[TEST03] Basic Test RESERVED KEY WORDs\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    printf("Tokens:\n");

    while (1) {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    printf("Expected tokens:\n 2\n 16\n 2\n 12\n 2\n 5\n\n");

    fclose(my_source);
}

void TEST04(FILE* my_source, tToken* next_token) {
    my_source=fopen("test4", "r");

    printf("[TEST04] Test correct identification \n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    printf("Tokens:\n");

    while (1) {
        getNextToken(next_token, my_source);
        printf(" %d \t-> ",next_token->type);

        if (next_token->type == IDENTIFIER_TOK) {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }
        else if (next_token->type == INTEGER_TOK) {
            printf("%d\n", next_token->attribute.number);
        }
        else if (next_token->type == FLOATING_POINT_TOK) {
            printf("%f\n", next_token->attribute.float_number);
        }
        else {
            printf("%s\n", next_token->attribute.string_ptr);
            stringFree(next_token->attribute.string_ptr);
            free(next_token->attribute.string_ptr);
        }

        if (next_token->type == EOF_TOK) {
            break;
        }
    }

    printf("Expected tokens:\n 2\n 16\n 2\n 12\n 2\n 5\n\n");

    fclose(my_source);
}

int main (int argc, char* argv[]) {
    FILE* my_source;
    tToken* next_token;

    printf ("---------------------IFJ Scanner - Tests-----------------------\n");
    printf ("---------------------------------------------------------------\n\n");

    next_token = (tToken*) malloc(sizeof(tToken));

    TEST01(my_source, next_token);
    TEST02(my_source, next_token);
    TEST03(my_source, next_token);
    TEST04(my_source, next_token);

    free(next_token);
    printf("\n----- Scanner - The End of Tests -----\n");

    return(0);
}
