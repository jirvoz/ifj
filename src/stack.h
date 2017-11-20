#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>
#include "scanner.h"
#include "expressions.h"
    
//structure of Stack with dynamic size
typedef struct tStack 
{
    tTerm* arr;        //data
    int size;           //actual size of stack
    int top;            //index of item on the top of stack
} tStack;

//declarations of functions

tStack* stackInit();
bool stackEmpty(tStack* stack);
void stackPush(tStack* stack, tTerm term);
tTerm* stackPop(tStack* stack);
tTerm* stackTop(tStack* stack);
void stackFree(tStack* stack);

#endif
