#include <stdlib.h>
#include <string.h>
#include "symtable.h"

// Hash function using alogorithm djb2 by Dan Bernstein
unsigned hashCode (char* name)
{
    unsigned hash = 5381;
    int c;

    while ((c = *name++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % HTSIZE;
}

//initializaton of hash table before first use
void htInit (tHtable* ptrht)
{
    for (int i = 0; i < HTSIZE; i++)
    {
        ptrht[i] = NULL;
    }
}

//searching function
tSymbol* htSearch ( tHtable* ptrht, char* name ) 
{
    tHtitem* ptr = ptrht[hashCode(name)];

    while (ptr != NULL)
    {
        if (strcmp(ptr->name, name) == 0)
        {
            return &ptr->symbol;
        }
        ptr = ptr->next;
    }
    return NULL;
}

//function insert item in the table
void htInsert (tHtable* ptrht, char* name, tSymbol symbol)
{
    tHtitem* ptr = NULL;

    int code = hashCode(name);

    ptr = malloc (sizeof(tHtitem));
    ptr->next = ptrht[code];
    ptrht[code] = ptr;

    ptr->name = name;
    ptr->symbol = symbol;
    
}

//delete item from table
void htDelete (tHtable* ptrht, char* name)
{
    int code = hashCode(name);
    tHtitem* ptr = ptrht[code];
    tHtitem* tmp;

    if (ptr != NULL)
    {
        if (strcmp(ptr->name, name) == 0)
        {
            tmp = ptr->next;
            free(ptr->name);
            free(ptr->symbol.arg_types);
            for (int j = 0; j < ptr->symbol.arg_count; j++)
                free(ptr->symbol.arg_names[j]);
            free(ptr->symbol.arg_names);
            free(ptr);
            ptrht[code] = tmp;
            return;
        }
        while (ptr->next != NULL)
        {
            if (ptr->next->name == name)
            {
                tmp = ptr->next;
                ptr->next = ptr->next->next;
                free(tmp->name);
                free(tmp->symbol.arg_types);
                for (int j = 0; j < tmp->symbol.arg_count; j++)
                    free(tmp->symbol.arg_names[j]);
                free(tmp->symbol.arg_names);
                free(tmp);
                tmp = NULL;
            }
            ptr = ptr->next;
        }
    }
}

//clear whole hash table
void htClearAll (tHtable* ptrht)
{
    tHtitem* ptr;
    for (int i = 0; i < HTSIZE; i++) 
    {
        if (ptrht[i] != NULL) 
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
}
