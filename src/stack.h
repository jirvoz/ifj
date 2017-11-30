#ifndef _STACK_H_
#define _STACK_H_

#include "expressions.h"
#include "scanner.h"

// Structure of Stack with dynamic size
typedef struct tStack 
{
    struct tTerm* arr;        //data
    int size;           //actual size of stack
    int top;            //index of item on the top of stack
} tStack;

// Stack functions
tStack* stackInit();
bool stackEmpty(tStack* stack);
void stackPush(tStack* stack, struct tTerm term);
struct tTerm* stackPop(tStack* stack);
struct tTerm* stackTop(tStack* stack);
void stackFree(tStack* stack);

#endif
