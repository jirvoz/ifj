#include <malloc.h>
#include "errors.h"
#include "expressions.h"

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

    stack->arr = malloc(sizeof(tTerm) * 8);
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
    if (stack->top + 1 <= stack->size)
    {
        stack->arr = realloc(stack->arr, (sizeof(tTerm) * (stack->size + 8)));
        if (stack->arr == NULL) 
        {
            addError(OTHER_ERROR, "Memmory allocation error");
            free(stack);
            return;
        }
        stack->size += 8;
    }
    stack->top++;
    stack->arr[stack->top] = term;
}

//delete item from the top of stack
tTerm stackPop(tStack* stack)
{
    if(!stackEmpty(stack)) 
    {
        (stack->top)--;
        return stack->arr[(stack->top) + 1];
    }   
    else
    {
        tTerm newTerm;
        newTerm.index = DOLAR_IN;
        newTerm.token.type = UNDEFINED_TOK;
        return newTerm;
    }
}

//save the pointer of the item from the top of stack
tTerm stackTop(tStack* stack) 
{
    if (!stackEmpty(stack))
        return stack->arr[stack->top];
    else
    {
        tTerm newTerm;
        newTerm.index = DOLAR_IN;
        newTerm.token.type = UNDEFINED_TOK;
        return newTerm;
    }
}

// Free alocated memory of arr and whole stack
void stackFree(tStack* stack) 
{
    if (stack != NULL)
    {
        if (stack->arr != NULL)
            free(stack->arr);
        free(stack);
    }
}
