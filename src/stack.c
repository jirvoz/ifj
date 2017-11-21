#include <malloc.h>
#include "errors.h"
#include "stack.h"

//initialization of stack - size is 8
//top index is -1 => stack is empty
tStack* stackInit() 
{
    tStack* stack;
    stack = malloc(sizeof(tStack));

    if (stack == NULL) 
    {
        addError(OTHER_ERROR, "Memmory allocation error");
        return NULL;
    }

    stack->arr = malloc(sizeof(tToken) * 8);
    if (stack->arr == NULL) 
    {
        addError(OTHER_ERROR, "Memmory allocation error");
        free(stack);
        return NULL;
    }

    stack->size = 8;
    stack->top = -1;
    return stack;
}

//if stack is empty return true
bool stackEmpty(tStack* stack) 
{
    return (stack->top == -1);
}

//push item to the top of stack
//if the stack is full, is resized
void stackPush(tStack* stack, tTerm term) 
{
    if (stack->size < (stack->top)) 
    {
        stack->arr = realloc(stack->arr, (sizeof(tToken) * (stack->size + 8)));
        if (stack->arr == NULL) 
        {
            addError(OTHER_ERROR, "Memmory allocation error");
            free(stack);
            return;
        }
        stack->size += 8;
    }
    (stack->top)++;
    stack->arr[stack->top] = term;
}

//delete item from the top of stack
tTerm* stackPop(tStack* stack)
{
    if(!stackEmpty(stack)) 
    {
        (stack->top)--;
        return &(stack->arr[(stack->top) + 1]);
    }   
    else 
    {
        return NULL;
    }
}

//save the pointer of the item from the top of stack
tTerm* stackTop(tStack* stack) 
{
    if (!stackEmpty(stack)) 
    {
        return &(stack->arr[stack->top]);
    }
    else
    {
        return NULL;
    }
}

//free alocated memory and set size to 0
void stackFree(tStack* stack) 
{
    free(stack->arr);
    stack->size = 0;
    stack->top = -1;
}
