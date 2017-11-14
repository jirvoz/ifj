#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>
#include "scanner.h"

    
//structure of Stack with dynamic size
typedef struct tStack 
{
    tToken* arr;        //data
    int size;           //actual size of stack
    int top;            //index of item on the top of stack
} tStack;

//declarations of functions

tStack* stackInit();
bool stackEmpty(tStack* stack);
void stackPush(tStack* stack, tToken* token);
tToken* stackPop(tStack* stack);
tToken* stackTop(tStack* stack);
void stackFree(tStack* stack);

#endif