#ifndef _POSTFIX_LIST_H
#define _POSTFIX_LIST_H

#include "scanner.h"

typedef struct tList_item 
{
    tToken token;
    struct tList_item* next;
} tList_item;
    
typedef struct tList
{
    tList_item* first;       //pointer to first item
    tList_item* last;        //pointer to last item
    tList_item* active;      //pointer to active item
} tList;

void listInit(tList* list);
void listFree(tList* list);
void listInsertLast(tList* list, tToken token);
void listFirst(tList* list);
void listNext(tList* list);
void listGoTo(tList* list, tToken* token);
tToken* listGetPointerLast(tList *list);
tToken* listGetData(tList* list);

#endif
