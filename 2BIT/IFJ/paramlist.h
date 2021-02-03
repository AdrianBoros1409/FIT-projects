/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Parameter list header
 * @file paramlist.h
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define TRUE 1
#define FALSE 0

// List item structure
typedef struct tElem {
    struct tElem *ptr;
    char* param;
} *tElemPtr;

// List structure
typedef struct {
    tElemPtr Act;
    tElemPtr First;
    int Count;
} tList;

// Function initializes the list
void InitList (tList *);

// Function frees all resources used by paramList
void DisposeList (tList *);

// Function inserts an item to the beginning of the list
int InsertFirst (tList *, char *);

// Function appends an item to the list
int PostInsert (tList *, char *);

// Function sets the actual item pointer to the first item
void ListFirst (tList *);

// Function moves the actual item pointer to its successor
void ListSucc (tList *);
