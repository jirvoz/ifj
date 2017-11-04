#include "errors.h"
#include <stdio.h>

unsigned error_count = 0;

char* err_message[] = {
    "Nastala chyba v programu v rámci lexikální analýzy\n",
    "Nastala chyba v programu v rámci syntaktické analýzy\n",      
    "Nastala sémantická chyba v programu – nedefinovaná funkce/promenná, pokus o redefinici funkce/promenné, atd\n",
    "Nastala sémantická chyba typové kompatibility v aritmetických,retezcových a relacních výrazech, príp. špatný pocet ci typ parametru u volání funkce\n",
    "Nastala jiná sémantická chyba\n",
    "Nastala chyba při alokaci paměti\n"
}; 

void addError (unsigned line, err_code code) {
    if (code == 6) {
        fprintf(stderr, err_message[4]);
    }
    else if (code == 99) {
        fprintf(stderr, err_message[5]);
       
    }
    else {
        fprintf(stderr, err_message[code - 1]);
    }
    fprintf(stderr, "Řádek: %u\n", line); 
}

