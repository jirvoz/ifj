#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include <stdbool.h>
#include "scanner.h"

//rows and cols in precedence table
typedef enum p_table_index
{
    EQ_EXPR_IN,
    NOT_EQ_IN,
    LESS_EQ_IN,
    MORE_EQ_IN,
    LESS_IN,
    MORE_IN,
    PLUS_IN,
    MINUS_IN,
    MUL_IN,
    FLOAT_DIV_IN,
    INT_DIV_IN,
    LEFT_PARENT_IN,
    RIGHT_PARENT_IN,
    INT_IN,                 //13
    DOUBLE_IN,
    STRING_IN,
    EQ_COMMAND_IN,
    UNARY_MINUS_IN,
    DOLAR_IN
} p_table_index;

/***********************LIST structures*****************************/

typedef struct tList_item 
{
    tToken token;
    p_table_index index;
    struct tList_item* next;
} tList_item;
    
typedef struct tList
{
    tList_item* first;       //pointer to first item
    tList_item* last;        //pointer to last item
    tList_item* active;      //pointer to active item
} tList;

// Parsing of expressions
// expected_type - type of variable, that should end on the top of stack
//                 can be INTEGER, DOUBLE, STRING, BOOL
//                 or UNDEFINED_TOK = 0 (no specific type needed)
bool expression(token_type expected_type);

tList* getInfix(token_type expected_type);
bool getTerm(tToken token, p_table_index* index);
tList infixToPostfix(tList* list_infix);
void generateInstructions(token_type expected_type, tList* list);

/*********************LIST functions*******************************/

void listInit(tList* list);
void listFree(tList* list);
void listInsertLast(tList* list, tToken token, p_table_index index);
void listFirst(tList* list);
void listNext(tList* list);
void listGoTo(tList* list, tList_item* token);
tList_item* listGetPointerLast(tList* list);
tList_item* listGetData(tList* list);

#endif
