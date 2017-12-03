#include <stdlib.h>
#include <string.h>
#include "symtable.h"

// Hash function using alogorithm djb2 by Dan Bernstein
unsigned hashCode(char* name)
{
    unsigned hash = 5381;
    int c;

    while ((c = *name++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % HTSIZE;
}

// Allocation and initializaton of hash table
tHTable* htInit()
{
    tHTable* ht =  malloc(sizeof(tHTItem) * HTSIZE);

    if (!ht)
        return NULL;

    for (int i = 0; i < HTSIZE; i++)
        ht[i] = NULL;

    return ht;
}

// Search for symbol data of specified name
tSymbol* htSearch(tHTable* ptrht, char* name)
{
    tHTItem* ptr = ptrht[hashCode(name)];

    while (ptr != NULL)
    {
        if (strcmp(ptr->name, name) == 0)
            return &ptr->symbol;
        ptr = ptr->next;
    }

    return NULL;
}

// Insert new item to table
void htInsert(tHTable* ptrht, char* name, tSymbol symbol)
{
    tHTItem* ptr = NULL;

    int code = hashCode(name);

    ptr = malloc (sizeof(tHTItem));
    ptr->next = ptrht[code];
    ptrht[code] = ptr;

    ptr->name = name;
    ptr->symbol = symbol;
    
}

// Remove all items from table
void htClear(tHTable* ptrht)
{
    tHTItem* ptr;
    for (int i = 0; i < HTSIZE; i++)
    {
        while (ptrht[i] != NULL)
        {
            ptr = ptrht[i];
            ptrht[i] = ptr->next;
            free(ptr->name);
            free(ptr->symbol.arg_types);
            for (int j = 0; j < ptr->symbol.arg_count; j++)
                free(ptr->symbol.arg_names[j]);
            free(ptr->symbol.arg_names);
            free(ptr);
            ptr = NULL;
        }
    }
}

// Destroy and free the table itself
void htFree(tHTable* ptrht)
{
    htClear(ptrht);
    free(ptrht);
}
