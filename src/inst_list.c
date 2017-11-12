#include <stdio.h>
#include <malloc.h>
#include "inst_list.h"
#include "errors.h"
#include "scanner.h"

//list initialization
void listInit(tListOfInst* list) {
    list->first  = NULL;
    list->last   = NULL;
    list->active = NULL;
}
  
//list dealocation
void listFree(tListOfInst* list) {
    tListItem* ptr;
    while (list->first != NULL) {
        ptr = list->first;
        list->first = list->first->nextItem;

        //free whole item
        free(ptr);
    }
}

//insert new item to end of the list
void listInsertLast(tListOfInst* list, tInst instruction) {
    tListItem *newItem;
    newItem = malloc(sizeof (tListItem));
    newItem->instruction = instruction;
    newItem->nextItem = NULL;

    if (list->first == NULL) {
        list->first = newItem;
    }
    else {
        list->last->nextItem = newItem;
    }
    list->last = newItem;
}

//activate first item
void listFirst(tListOfInst* list) {
    list->active = list->first;
}

//activate item after actual activated item
void listNext(tListOfInst* list) {
    if (list->active != NULL) {
        list->active = list->active->nextItem;   
    }
}

//activate instruction on pointer
void listGoto(tListOfInst* list, void* gotoInst) {
    list->active = (tListItem*) gotoInst;
}

//return pointer of last item 
void* listGetPointerLast(tListOfInst* list) {
    return (void*) list->last;
}

//return active instruction
tInst *listGetData(tListOfInst* list) {
    if (list->active == NULL) {
        addError(line, OTHER_ERROR);
        return NULL;
    }
    else {
        return &(list->active->instruction);  
    } 
}
