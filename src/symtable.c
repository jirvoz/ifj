#include <stdlib.h>
#include <string.h>

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void setData (tData data, tHTItem* ptr) {
	ptr->data->var_name = data->var_name;
	ptr->data->init_flag = data->init_flag;
	ptr->data->var->type = data->var->type;
	
	if (data->var->type == INTEGER_TOK) {
		ptr->data->var->attribute->number = data->var->attribute->number; 
	} 
	else if (data->var->type == FLOATING_POINT_TOK) {
		ptr->data->var->attribute->float_number = data->var->attribute->float_number;	
	}
	else {
		ptr->data->var->attribute->string_ptr = data->var->attribute->string_ptr;
	}
}

void htInit ( tHTable* ptrht ) {
	for (int i = 0; i < MAX_HTSIZE; i++) {
		(*ptrht)[i] = NULL; 
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	//int i = hashCode(key);
	tHTItem* ptr = (*ptrht)[hashCode(key)];

	while (ptr != NULL) {
		if (!strcmp(ptr->key, key)) {
			return ptr;
		}
		ptr = ptr->ptrnext;
	}
	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	tHTItem* ptr = NULL;

	if ((ptr = htSearch(ptrht, key)) != NULL) {
		setData(data, ptr);
	}
	else {
		ptr = malloc (sizeof(tHTItem));
		ptr->ptrnext = (*ptrht)[hashCode(key)];
		(*ptrht)[hashCode(key)] = ptr;
		ptr->key = key;
		setData(data, ptr);
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	tHTItem* ptr;
	if ((ptr = htSearch(ptrht, key)) != NULL) {
		return &(ptr->data);
	}
	else {
		return NULL;
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
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

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
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
