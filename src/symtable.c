#include <stdlib.h>
#include <string.h>
#include "symtable.h"

//hash function
int hashCode (char* name)
{
    unsigned int h = 0;
    unsigned char *p;

    for(p = (unsigned char*)name; *p != '\0'; p++)
        h = 65599 * h + *p;

    return h % HTSIZE;
}

//initializaton of hash table before first use
void htInit (tHtable* ptrht)
{
    for (int i = 0; i < HTSIZE; i++)
    {
        (*ptrht)[i] = NULL;
    }
}

//searching function
tHtitem* htSearch ( tHtable* ptrht, char* name ) 
{
    tHtitem* ptr = (*ptrht)[hashCode(name)];

    while (ptr != NULL)
    {
        if (!strcmp(ptr->name, name))
        {
            return ptr;
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
    ptr->next = (*ptrht)[code];
    (*ptrht)[code] = ptr;

    ptr->name = name;
    ptr->symbol = symbol;
    
}

//delete item from table
void htDelete (tHtable* ptrht, char* name)
{
    int code = hashCode(name);
    tHtitem* ptr = (*ptrht)[code];
    tHtitem* tmp;

    if (ptr != NULL)
    {
        if (!strcmp(ptr->name, name))
        {
            tmp = ptr->next;
            free(ptr);
            (*ptrht)[code] = tmp;
            return;
        }
        while (ptr->next != NULL)
        {
            if (ptr->next->name == name)
            {
                tmp = ptr->next;
                ptr->next = ptr->next->next;
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
        if ((*ptrht)[i] != NULL) 
        {
            while ((*ptrht)[i] != NULL) 
            {
                ptr = (*ptrht)[i];
                (*ptrht)[i] = ptr->next;
                free(ptr->name);
                free(ptr->symbol.args);
                free(ptr);
                ptr = NULL;
            }
        }
    }
}
