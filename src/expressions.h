#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include <stdbool.h>
#include "scanner.h"
//#include "stack.h"

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

//structure sent to generateInstruction
//data of stack
typedef struct tTerm
{
    tToken token;
    enum p_table_index index;
} tTerm;

//structure of Stack with dynamic size
typedef struct tStack 
{
    struct tTerm* arr;        //data
    int size;           //actual size of stack
    int top;            //index of item on the top of stack
} tStack;

// Parsing of expressions
// expected_type - type of variable, that should end on the top of stack
//                 can be INTEGER, DOUBLE, STRING, BOOLEAN
//                 or UNDEFINED_TOK = 0 (no specific type needed)
bool expression(token_type expected_type);

//other functions
bool postfix(token_type expected_type, token_type return_type, struct tTerm* term, struct tStack* stack);
bool getTerm(tTerm* term);
bool getPriority (tTerm* term, tStack* stack, token_type return_type);
bool generateInstruction(token_type return_type, tTerm sent_term);
void memoryClear(tTerm* term, tStack* stack);

//just for tests
//void printTerm(tTerm term);


//stack functions
tStack* stackInit();
bool stackEmpty(tStack* stack);
void stackPush(tStack* stack, struct tTerm term);
struct tTerm* stackPop(tStack* stack);
struct tTerm* stackTop(tStack* stack);
void stackFree(tStack* stack);

#endif
