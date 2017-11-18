#include <stdio.h>
#include <malloc.h>
#include "postfix_list.h"
#include "errors.h"
#include "scanner.h"

//list initialization
void listInit(tList* list)
{
    list->first  = NULL;
    list->last   = NULL;
    list->active = NULL;
}
  
//list dealocation
void listFree(tList* list)
{
    tList_item* ptr;
    while (list->first != NULL)
    {
        ptr = list->first;
        list->first = list->first->next;

        //free whole item
        free(ptr);
    }
}

//insert new item to end of the list
void listInsertLast(tList* list, tToken token, p_table_index index)
{
    tList_item *new_item;
    new_item = malloc(sizeof (tList_item));
    new_item->token = token;
    new_item->index = index;
    new_item->next = NULL;

    if (list->first == NULL)
    {
        list->first = new_item;
    }
    else
    {
        list->last->next = new_item;
    }
    list->last = new_item;
}

//activate first item
void listFirst(tList* list)
{
    list->active = list->first;
}

//activate item after actual activated item
void listNext(tList* list)
{
    if (list->active != NULL)
    {
        list->active = list->active->next;   
    }
}

//activate token on pointer
void listGoto(tList* list, tList_item* item)
{
    list->active = item;
}

//return pointer of last item 
tList_item* listGetPointerLast(tList* list)
{
    return list->last;
}

//return active item
tList_item* listGetData(tList* list)
{
    if (list->active == NULL)
    {
        addError(OTHER_ERROR, "Internal error");
        return NULL;
    }
    else
    {
        return list->active;  
    } 
}
