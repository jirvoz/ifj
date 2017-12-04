//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        stack.c
//  Description: Source file of abstract structure stack
//               All stack operations are implemented in this module  
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include <stdlib.h>
#include "expressions.h"
// "stack.h" is included in "expression.h"
#include "errors.h"

// Initialization of stack
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

// Find out if stack is empty
bool stackEmpty(tStack* stack) 
{
    return (stack->top == -1);
}

// Push item to the top of stack
void stackPush(tStack* stack, tTerm term) 
{
    if (stack->top + 1 <= stack->size)
    {
        // New size is old size + 8 
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

// Delete item from the top of stack
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

// Return item from the top of stack
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

// Free alocated memory of arr and stack
void stackFree(tStack* stack) 
{
    if (stack != NULL)
    {
        if (stack->arr != NULL)
            free(stack->arr);
        free(stack);
    }
}
