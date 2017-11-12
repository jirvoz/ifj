#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdbool.h>
#include "scanner.h"

typedef enum var_type {
    INTEGER_TYPE,
    FLOATING_POINT_TYPE,
    STRING_TYPE
} var_type;

typedef union tVar {
    var_type type;
    tToken_attribute attribute;
} tVar;

typedef struct tVar_data {
    char* var_name;
    bool init_flag;
    tVar var_data;
} tVar_data;

typedef struct tFunc_data {
    char* func_name;
    bool decl_flag;
    bool def_flag;
    tVar ret_value;
    int param_count;
    tVar* params;
} tFunc_data;

typedef union tData {
    tVar_data variable;
    tFunc_data function;
} tData;

typedef enum symbol_type {
    VARIABLE_SYMBOL,
    FUNCTION_SYMBOL
} symbol_type;

//max size of hash table
#define MAX_HTSIZE 101

//this structure represent symbol in hash table
 typedef struct tHTItem{
    char* key;                  //key
    symbol_type type;           //type of symbol 
    tData data;                 //data of symbol
    struct tHTItem* ptrnext;    //pointer to next synonymum
} tHTItem;

//hash table
typedef tHTItem* tHTable[MAX_HTSIZE];


//Declarations of functions

int hashCode ( char* key );

void setData (tData data, tHTItem* ptr);

void htInit ( tHTable* ptrht );

tHTItem* htSearch ( tHTable* ptrht, char* key );

void htInsert ( tHTable* ptrht, char* key, tData data );

tData* htRead ( tHTable* ptrht, char* key );

void htDelete ( tHTable* ptrht, char* key );

void htClearAll ( tHTable* ptrht );

#endif