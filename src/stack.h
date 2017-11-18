#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>
#include "scanner.h"
#include "expressions.h"

typedef struct tStack_data
{
	tToken token;
	p_table_index index;

}tStack_data;
    
//structure of Stack with dynamic size
typedef struct tStack 
{
    tStack_data* arr;        //data
    int size;           //actual size of stack
    int top;            //index of item on the top of stack
} tStack;

//declarations of functions

tStack* stackInit();
bool stackEmpty(tStack* stack);
void stackPush(tStack* stack, tToken token, p_table_index index);
tStack_data* stackPop(tStack* stack);
tStack_data* stackTop(tStack* stack);
void stackFree(tStack* stack);

#endif
