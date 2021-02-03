/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Main implementation
 * @file main.c
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Matouš Sloboda <xslobo04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include "main.h"


int main() {
    /*FILE *f;
    if (argc == 1) {
        printf("Nie je zadany vstupny subor\n");
        return FILE_ERROR;
    }
    if ((f = fopen(argv[1], "r")) == NULL) {
        printf("Subor se nepodarilo otvorit\n");
        return FILE_ERROR;
    }*/
    setSource(stdin);

    // Hash tables for IDs
    tHTable lsTable;
    htInit(&lsTable);
    tHTable gsTable;
    htInit(&gsTable);

    int result;
    result = parse(&lsTable, &gsTable);

    // Free memory resources
    htClearAll(&gsTable);
    htClearAll(&lsTable);

    //fclose(f);

    return result;
}
