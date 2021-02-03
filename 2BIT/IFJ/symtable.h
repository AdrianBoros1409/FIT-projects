/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Symtable header
 * @file symtable.h
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @brief Murmur2: https://github.com/abrandoned/murmur2
 */

#ifndef IFJ_SYMTABLE_H
#define IFJ_SYMTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"
#include "string.h"
#include "paramlist.h"

// Hash table size
#define MAX_HT_SIZE 8641

typedef enum {
    dtUndefined,
    dtInt,
    dtFloat,
    dtString,
} dType;

typedef char* tKey;

// Hash table item data
typedef struct {
    dType type;
    tList* parameters;
    tKey identifier;
    bool isFunct;
    bool isParam;
    bool inIf;
} tData;

// Hash table item
typedef struct tHTItem {
    tKey key; /// identifier
    tData data; /// data
    struct tHTItem *ptrnext; /// ptr to next item
} tHTItem;

// Hash table
typedef tHTItem* tHTable[MAX_HT_SIZE];

extern int HTSIZE;

// Function initializes the hash table
void htInit ( tHTable* ptrht );

// Function returns an item from hash table if found, otherwise return NULL
tHTItem* htSearch ( tHTable* ptrht, tKey key );

// Function insert an item to hash table with specified data and returns a pointer to it if it succeeds
tData* htInsert ( tHTable* ptrht, tKey key, bool* status , bool inIfThen);

// Function return data about an item in the hash table
tData* htRead ( tHTable* ptrht, tKey key );

// Function frees all resources used by hash table, even paramList
void htClearAll ( tHTable* ptrht );

void htClearLocal ( tHTable* ptrht);

#endif //IFJ_SYMTABLE_H
