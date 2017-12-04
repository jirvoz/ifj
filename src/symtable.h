#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "scanner.h"

//size of hash table
#define HTSIZE 128

// Data of the symbol
typedef struct tSymbol
{
    //for variable and function
    token_type type;            // Type of variable (or return type of function)

    //for function only
    bool defined;               // Function defined flag
    int arg_count;              // Function arguments count
    int arg_size;               // Size of arrays of function arguments
    token_type* arg_types;      // Types of function arguments
    char** arg_names;           // Names of function arguments
} tSymbol;

// Single item from hash table
typedef struct tHTItem
{
    char* name;                 // Name is also key
    tSymbol symbol;             // Data of symbol
    struct tHTItem* next;       // Pointer to the next synonym
} tHTItem;

// Hash table main structure
typedef tHTItem* tHTable;


// Hash function (using alogorithm djb2 by Dan Bernstein)
unsigned hashCode(char* name);

// Allocation and initializaton of hash table
tHTable* htInit();

// Search for symbol data of specified name
tSymbol* htSearch(tHTable* ptrht, char* name);

// Insert new item to table
void htInsert(tHTable* ptrht, char* name, tSymbol symbol);

// Remove all items from table
void htClear(tHTable* ptrht);

// Destroy and free the table itself
void htFree(tHTable* ptrht);

#endif
