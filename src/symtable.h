#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "scanner.h"

//size of hash table
#define HTSIZE 128

typedef struct tSymbol
{
    //for variable and function
    token_type type;            //type of variable (or return type of function)

    //for function only
    bool defined;               //function defined flag
    int arg_count;              //function arguments count
    int arg_size;               //size of arrays of function arguments
    token_type* arg_types;      //types of function arguments
    char** arg_names;           //names of function arguments
} tSymbol;

//this structure represent symbol in hash table
typedef struct tHtitem
{
    char* name;                 //name is also key
    tSymbol symbol;             //data of symbol
    struct tHtitem* next;       //pointer to the next synonym
} tHtitem;

//hash table
typedef tHtitem* tHtable;

//Declarations of functions
unsigned hashCode(char* name);
void htInit(tHtable* ptrht);
tSymbol* htSearch(tHtable* ptrht, char* name);
void htInsert(tHtable* ptrht, char* name, tSymbol symbol);
void htDelete(tHtable* ptrht, char* name);
void htClearAll(tHtable* ptrht);

#endif
