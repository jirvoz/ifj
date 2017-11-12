#include <stdlib.h>
#include <string.h>
#include "symtable.h"

int HTSIZE = MAX_HTSIZE;
int solved;

//hash function

int hashCode (char* key) {
    int retval = 0;
    int keylen = strlen(key);
    for ( int i=0; i<keylen; i++ )
        retval += key[i];
    return ( retval % HTSIZE );
}

//initializaton of hash table before first use
void htInit (tHTable* ptrht) {
    for (int i = 0; i < MAX_HTSIZE; i++) {
        (*ptrht)[i] = NULL; 
    }
}

//this function set data of hash table item
void setSymbol (symbol_type type, tData data, tHTItem* ptr) {
    ptr->type = type;
    ptr->data = data;
}

//searching function
tHTItem* htSearch ( tHTable* ptrht, char* key ) {
    tHTItem* ptr = (*ptrht)[hashCode(key)];

    while (ptr != NULL) {
        if (!strcmp(ptr->key, key)) {
            return ptr;
        }
        ptr = ptr->ptrnext;
    }
    return NULL;
}

//function insert item in the table
void htInsert (tHTable* ptrht, char* key, symbol_type type, tData data) {
    tHTItem* ptr = NULL;

    if ((ptr = htSearch(ptrht, key)) != NULL) {
        setData(type, data, ptr);
    }
    else {
        int code = hashCode(key);
        ptr = malloc (sizeof(tHTItem));
        ptr->ptrnext = (*ptrht)[code];
        (*ptrht)[code] = ptr;
        ptr->key = key;
        setData(type, data, ptr);
    }
}

//function returns data of item 
tData* htRead (tHTable* ptrht, char* key) {
    tHTItem* ptr;
    if ((ptr = htSearch(ptrht, key)) != NULL) {
        return &(ptr->data);
    }
    else {
        return NULL;
    }
}

//delete item from table
void htDelete (tHTable* ptrht, char* key) {
    int i = hashCode(key);
    tHTItem* ptr = (*ptrht)[i];
    tHTItem* tmp;

    if (ptr != NULL) {
        if (!strcmp(ptr->key, key)) {
            tmp = ptr->ptrnext;
            free(ptr);
            (*ptrht)[i] = tmp;
            return;
        }
        while (ptr->ptrnext != NULL) {
            if (ptr->ptrnext->key == key) {
                tmp = ptr->ptrnext;
                ptr->ptrnext = ptr->ptrnext->ptrnext;
                free(tmp);
                tmp = NULL;
            }
            ptr = ptr->ptrnext;
        }
    }
}

//clear whole hash table
void htClearAll (tHTable* ptrht) {
    tHTItem* ptr;
    for (int i = 0; i < MAX_HTSIZE; i++) {
        if ((*ptrht)[i] != NULL) {

            while ((*ptrht)[i] != NULL) {
                ptr = (*ptrht)[i];
                (*ptrht)[i] = ptr->ptrnext;
                free(ptr);
                ptr = NULL;
            }
        }
    }
}
