#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdbool.h>
#include "scanner.h"

//size of hash table
#define HTSIZE 128

typedef struct tSymbol
{
    //for var and function
    token_type type;            //type of variable(or return type of function)

    //for function only
    bool declared;              //function declared flag
    int arg_count;              //function arguments count
    token_type* args;          	//array of function arguments
} tSymbol;

//this structure represent symbol in hash table
typedef struct  tHtitem
{
    char* name;                 //name is also key
    tSymbol symbol;             //data of symbol
    struct tHtitem* next;       //pointer to the next synonym
} tHtitem;

//hash table
typedef tHtitem* tHtable[HTSIZE];


//Declarations of functions

int hashCode (char* name);

void htInit (tHtable* ptrht);

tHtitem* htSearch (tHtable* ptrht, char* name);

void htInsert (tHtable* ptrht, char* name, tSymbol symbol);

void htDelete (tHtable* ptrht, char* name);

void htClearAll (tHtable* ptrht);

#endif
