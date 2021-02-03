/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Parameter list implementation
 * @file paramlist.c
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 */

#include "paramlist.h"

void InitList (tList *L) {
    L->Act = NULL;
    L->First = NULL;
    L->Count = 0;
}

void DisposeList (tList *L) {
    L->Act = NULL;

    tElemPtr tmp;

    while(L->First != NULL) {
        tmp = L->First;
        L->First = L->First->ptr;
        free(tmp);
    }
}

int InsertFirst (tList *L, char* val) {
    tElemPtr newElem = malloc(sizeof(struct tElem));

    if (newElem == NULL) {
        return 99;
    } else {
        newElem->param = val;
        newElem->ptr = L->First;
        L->First = newElem;
        L->Act = L->First;
        L->Count = 1;
    }
    return 0;
}

int PostInsert (tList *L, char* val) {
    if (L->Act != NULL) {
        tElemPtr newElem = malloc(sizeof(struct tElem));

        if (newElem == NULL) {
            return 99;
        } else {
            newElem->param = val;
            newElem->ptr = L->Act->ptr;
            L->Act->ptr = newElem;
            L->Act = L->Act->ptr;
            L->Count++;
        }
    }
    return 0;
}

void ListFirst (tList *L) {
    L->Act = L->First;
}

void ListSucc (tList *L) {
    if (L->Act != NULL) {
        if (L->Act->ptr != NULL) {
            L->Act = L->Act->ptr;
        } else {
            L->Act = NULL;
        }
    }
}
