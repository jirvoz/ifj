//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        statements.c
//  Description: Source file of statement parsing
//               Rules for parsing statements
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "expressions.h"
#include "functions.h"
#include "parser.h"
#include "scanner.h"
#include "statements.h"
#include "symtable.h"

bool dim_stat()
{
    // last_token.type is DIM

    UPDATE_LAST_TOKEN();

    // Check for identifier token
    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier after DIM.");

    char * identif_name = last_token.attribute.string_ptr;

    // Check for collision with function name
    if (htSearch(func_table, identif_name))
        ERROR_AND_RETURN(SEM_PROG_ERROR, "There is already function named '%s'.", identif_name);

    // Check for redefinition
    if (htSearch(var_table, identif_name))
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Variable '%s' is already defined.", identif_name);

    printf("DEFVAR LF@%s\n", identif_name);

    UPDATE_LAST_TOKEN();

    // Check for AS token
    if (last_token.type != AS)
        ERROR_AND_RETURN(SYN_ERROR, "Expected AS after identifier.");

    UPDATE_LAST_TOKEN();

    token_type type = last_token.type;

    UPDATE_LAST_TOKEN();

    // If there is no declaration
    if (last_token.type == EOL_TOK)
    {
        switch (type)
        {
            case INTEGER:
                printf("MOVE LF@%s int@0\n", identif_name);
                break;
            case DOUBLE:
                printf("MOVE LF@%s float@0\n", identif_name);
                break;
            case STRING:
                printf("MOVE LF@%s string@\n", identif_name);
                break;
            default:
                ERROR_AND_RETURN(SYN_ERROR, "Expected variable type after AS.");
        }

        // Allocate space for copy of variable name including space for '\0'
        char* new_name = malloc((strlen(identif_name) + 1) * sizeof(char));
        new_name = strcpy(new_name, identif_name);

        htInsert(var_table, new_name,
            (tSymbol){ .type=type, .defined = true, .arg_count = 0 });

        free(identif_name);
        return true;
    }
    // Check for optional assignment
    else if (last_token.type == EQUAL_SIGN_OP)
    {
        // Evaluate expression
        UPDATE_LAST_TOKEN();
        if (!expression(type))
            return false;

        printf("POPS LF@%s\n", identif_name);

        // Allocate space for copy of variable name including space for '\0'
        char* new_name = malloc((strlen(identif_name) + 1) * sizeof(char));
        new_name = strcpy(new_name, identif_name);

        // Add record to symbol table
        htInsert(var_table, new_name,
            (tSymbol){ .type=type, .defined = true, .arg_count = 0 });

        free(identif_name);

        // Check for EOL at the end of expression
        if (last_token.type != EOL_TOK)
            ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after assignment.");

        return true;
    }
    else
    {
        free(identif_name);
        ERROR_AND_RETURN(SYN_ERROR, "Expected assignment symbol '=' ",
            "or end of line after declaration.");
    }
}

bool assignment_stat()
{
    // last_token.type is IDENTIFIER_TOK

    char* identif_name = last_token.attribute.string_ptr;

    // Check symtable if variable exists
    tSymbol* symbol = htSearch(var_table, identif_name);
    if (!symbol)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Undefined variable at assignment.");

    UPDATE_LAST_TOKEN();

    // Check for equal sign
    if (last_token.type != EQUAL_SIGN_OP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected assignment symbol '=' after identifier.");
    
    // Evaluate expression
    UPDATE_LAST_TOKEN();
    if (!expression(symbol->type))
        return false;

    printf("POPS LF@%s\n", identif_name);
    free(identif_name);

    // Check for EOL at the end of expression
    if (last_token.type != EOL_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after assignment.");

    return true;
}

bool input_stat()
{
    // last_token.type is INPUT

    printf("WRITE string@?\\032\n");

    // Read variable to use for input
    UPDATE_LAST_TOKEN();

    if (last_token.type != IDENTIFIER_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected identifier after INPUT.");

    // Check symtable if variable exists
    tSymbol* symbol = htSearch(var_table, last_token.attribute.string_ptr);
    if (!symbol)
        ERROR_AND_RETURN(SEM_PROG_ERROR, "Undefined variable after INPUT.");

    // Write read instruction with first parameter
    printf("READ LF@%s ", last_token.attribute.string_ptr);

    // Write second parameter - type of input
    switch (symbol->type)
    {
        case INTEGER:
            printf("int\n");
            break;
        case DOUBLE:
            printf("float\n");
            break;
        case STRING:
            printf("string\n");
            break;
        case BOOLEAN:
            printf("bool\n");
            break;
        default:
            ERROR_AND_RETURN(OTHER_ERROR, "Unknown type of variable at input.");
    }

    free(last_token.attribute.string_ptr);

    // EOL after input statement
    UPDATE_LAST_TOKEN();

    if (last_token.type == EOL_TOK)
        return true;
    else
        ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after variable for input.");
}

bool print_stat()
{
    // last_token.type is PRINT

    UPDATE_LAST_TOKEN();
    do
    {
        if (!expression(UNDEFINED_TOK))
            return false;

        // Check semicolon after expression
        if (last_token.type != SEMICOLON_OP)
            ERROR_AND_RETURN(SYN_ERROR, "Expected semicolon after print value.");

        printf("CREATEFRAME\n");
        printf("DEFVAR TF@write\n");
        printf("POPS TF@write\n");
        printf("WRITE TF@write\n");

        // Read next expression or EOL after semicolon
        UPDATE_LAST_TOKEN();

        if (last_token.type == EOL_TOK)
            return true;
    }
    while (true);
}

bool if_stat()
{
    // last_token.type is IF

    unsigned if_line_number = line;
    unsigned end_line_number = line;

    // Evaluate condition
    UPDATE_LAST_TOKEN();
    if (!expression(BOOLEAN))
        return false;

    // Test token THEN after expression in if
    if (last_token.type != THEN)
        ERROR_AND_RETURN(SYN_ERROR, "Expected THEN after if expression.");

    // Write jump instruction
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS &else%d\n", if_line_number);

    // Do we expect also &else label at the end of conditions block?
    bool else_label = true;
    do
    {
        // Parse the statements inside of if block
        if (!statementList())
            return false;

        // Handle endings of if block
        switch (last_token.type)
        {
            case ELSE:
                else_label = false;
                printf("JUMP &endif%d\n", end_line_number);
                printf("LABEL &else%d\n", if_line_number);

                // Check the eol after else
                UPDATE_LAST_TOKEN();
                if (last_token.type != EOL_TOK)
                    ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after ELSE.");
                break;

            case ELSEIF:
                else_label = true;
                printf("JUMP &endif%d\n", end_line_number);
                printf("LABEL &else%d\n", if_line_number);

                // Evaluate next condition
                UPDATE_LAST_TOKEN();
                if (!expression(BOOLEAN))
                    return false;

                // Test token THEN after expression in if
                if (last_token.type != THEN)
                    ERROR_AND_RETURN(SYN_ERROR, "Expected THEN after elseif expression.");

                // Write jump instruction
                if_line_number = line;
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS &else%d\n", if_line_number);
                break;

            case END:
                if (else_label)
                    printf("LABEL &else%d\n", if_line_number);
                printf("LABEL &endif%d\n", end_line_number);

                // Check correct ending of if block
                UPDATE_LAST_TOKEN();
                if (last_token.type != IF)
                    ERROR_AND_RETURN(SYN_ERROR, "Bad ending of if block.");

                UPDATE_LAST_TOKEN();
                if (last_token.type != EOL_TOK)
                    ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after END IF.");

                return true;

            default:
                ERROR_AND_RETURN(SYN_ERROR, "Wrong ending of condition block.");
        }
    }
    while (true);
}

bool while_stat()
{
    // last_token.type is DO

    unsigned while_line_number = line;

    UPDATE_LAST_TOKEN();

    // Check if the next word is WHILE
    if (last_token.type != WHILE)
        ERROR_AND_RETURN(SYN_ERROR, "Expected WHILE after DO.");

    // Label beginning of while loop
    printf("LABEL &while%d\n", while_line_number);

    // Evaluate condition
    UPDATE_LAST_TOKEN();
    if (!expression(BOOLEAN))
        return false;

    // Check if expression ends with EOL
    if (last_token.type != EOL_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after while expression.");

    // Write jump instruction
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS &loop%d\n", while_line_number);

    // Parse the inside of do while
    if (!statementList())
        return false;

    printf("JUMP &while%d\n", while_line_number);
    printf("LABEL &loop%d\n", while_line_number);

    // Test the LOOP and eol at the end of loop
    if (last_token.type != LOOP)
        ERROR_AND_RETURN(SYN_ERROR, "Expected LOOP after do while block.");

    UPDATE_LAST_TOKEN();

    if (last_token.type != EOL_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after LOOP.");

    return true;
}

bool return_stat()
{
    // last_token.type is RETURN

    // Check if in function
    if (!actual_function)
        ERROR_AND_RETURN(SYN_ERROR, "RETURN can't be used outside of function.");

    // Call expression parsing
    tSymbol* func_symbol = htSearch(func_table, actual_function);

    UPDATE_LAST_TOKEN();
    if (!expression(func_symbol->type))
        return false;

    // Check if return expression ends with EOL
    if (last_token.type != EOL_TOK)
        ERROR_AND_RETURN(SYN_ERROR, "Expected end of line after while expression.");

    // Returned value is on the top of stack
    printf("POPFRAME\n");
    printf("RETURN\n");

    return true;
}
