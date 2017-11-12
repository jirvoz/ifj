#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdbool>

typedef enum tVarType {
    INTEGER_TYPE,
    FLOATING_POINT_TYPE,
    STRING_TYPE
} tVarType;

typedef union tVar {
    tVarType type;
    tToken_attribute attribute;
}

typedef struct tVarData {
    char* var_name;
    bool init_flag;
    tVar var_data;
} tVarData;

typedef struct tFuncData {
    char* func_name;
    bool decl_flag;
    bool def_flag;
    tVar ret_value;
    int param_count;
    tVar* params;
} tFuncData;

typedef union tData {
    tVarData variable;
    tFuncData function;
} tData;

typedef enum symbol_type {
    VARIABLE,
    FUNCTION
} symbol_type;

/* Maximální velikost pole pro implementaci
   vyhledávací tabulky. Řešené procedury však
   využívají pouze HTSIZE prvků pole (viz deklarace této proměnné).
*/
#define MAX_HTSIZE 101

/* typ klíče (například identifikace zboží) */
typedef char* tKey;

/*Datová položka TRP s explicitně řetězenými synonymy*/
 typedef struct tHTItem{
    tKey key;               /* klíč  */
    symbol_type type;       
    tData data;             /* obsah */
    struct tHTItem* ptrnext;    /* ukazatel na další synonymum */
} tHTItem;

/* TRP s explicitně zřetězenými synonymy. */
typedef tHTItem* tHTable[MAX_HTSIZE];

/* Pro účely testování je vhodné mít možnost volby velikosti pole,
   kterým je vyhledávací tabulka implementována. Fyzicky je deklarováno
   pole o rozměru MAX_HTSIZE, ale při implementaci vašich procedur uvažujte
   velikost HTSIZE.  Ve skriptu se před voláním řešených procedur musí
   objevit příkaz HTSIZE N, kde N je velikost požadovaného prostoru.
   
   POZOR! Pro správnou funkci TRP musí být hodnota této proměnné prvočíslem.
*/
extern int HTSIZE;

/* Hlavičky řešených procedur a funkcí. */

int hashCode ( tKey key );

void setData (tData data, tHTItem* ptr);

void htInit ( tHTable* ptrht );

tHTItem* htSearch ( tHTable* ptrht, tKey key );

void htInsert ( tHTable* ptrht, tKey key, tData data );

tData* htRead ( tHTable* ptrht, tKey key );

void htDelete ( tHTable* ptrht, tKey key );

void htClearAll ( tHTable* ptrht );

#endif