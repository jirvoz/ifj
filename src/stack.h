//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        stack.h
//  Description: Header file of abstract structure stack 
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#ifndef _STACK_H_
#define _STACK_H_

#include "expressions.h"
#include "scanner.h"

// Structure of Stack with dynamic size
typedef struct tStack 
{
    struct tTerm* arr; 	//data
    int size;           //actual size of stack
    int top;            //index of item on the top of stack
} tStack;

// Initialization of stack - size is 8
// top index is -1 => stack is empty
// returns pointer to created stack
tStack* stackInit();

// Find out if stack is empty
// if stack is empty return true
bool stackEmpty(tStack* stack);

// Push item to the top of stack
// if the stack is full, is resized
void stackPush(tStack* stack, struct tTerm term);

// Delete item from the top of stack
// returns item on the top of stack
// If stack is empty, returns term with Dolar index and undefined token
struct tTerm stackPop(tStack* stack);

// Return item from the top of stack
// If stack is empty, returns term with Dolar index and undefined token
struct tTerm stackTop(tStack* stack);

// Free alocated memory of arr and stack
// Do nothing if NULL
void stackFree(tStack* stack);

#endif
