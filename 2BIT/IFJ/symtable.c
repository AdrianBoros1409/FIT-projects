/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Symtable implementation
 * @file symtable.c
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @brief Murmur2: https://github.com/abrandoned/murmur2
 */

#include <string.h>
#include "symtable.h"

#define STR_LEN_INC 8

int HTSIZE = MAX_HT_SIZE;

// Implementation of Murmur2 hashing function
// Hash code was taken from: https://github.com/abrandoned/murmur2
unsigned int hashCode ( const void * key, int len ) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    const unsigned int seed = 7;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value
    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash
    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array
    switch(len)
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h % HTSIZE;
}

void htInit ( tHTable* ptrht ) {
    if (ptrht != NULL) {
        for (int i = 0; i < HTSIZE; i++) {
            (*ptrht)[i] = NULL;
        }
    }
}

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
    unsigned long index = hashCode(key, strlen(key));
    if ((*ptrht) == NULL || (*ptrht)[index] == NULL) {
        return NULL;
    } else {
        tHTItem *tmp = (*ptrht)[index];
        while (tmp != NULL) {
            if (strcmp(key, tmp->key) == 0) {
                return tmp;
            } else {
                tmp = tmp->ptrnext;
            }
        }
        return NULL;
    }
}

tData* htInsert ( tHTable* ptrht, tKey key, bool* status, bool inIfThen) {
    if(ptrht != NULL && key != NULL) {
        unsigned long index = hashCode(key, strlen(key));
        *status = true;
        tHTItem *tmp = htSearch(ptrht, key);

        if (tmp == NULL) {
            tHTItem *new = malloc(sizeof(tHTItem));
            if (new != NULL) {
                new->key = key;
                new->data.type = dtUndefined;
                new->data.identifier = new->key;
                new->data.parameters = NULL;
                new->data.isFunct = false;
                new->data.isParam = false;
                if (inIfThen)
                    new->data.inIf = true;
                else
                    new->data.inIf = false;
                new->ptrnext = (*ptrht)[index];
                (*ptrht)[index] = new;
            } else {
                *status = false;
                return NULL;
            }
            return &new->data;
        } else {
            tmp->data.identifier = tmp->key;
        }
        return &tmp->data;
    }
    else{
        *status = false;
        return NULL;
    }
}

tData* htRead ( tHTable* ptrht, tKey key ) {
    tHTItem *tmp = htSearch(ptrht, key);

    if (tmp != NULL) {
        return &tmp->data;
    } else {
        return NULL;
    }
}

void htClearAll ( tHTable* ptrht ) {
    if (ptrht != NULL) {
        tHTItem *tmp;
        for (int i = 0; i < HTSIZE; i++) {
            while ((*ptrht)[i] != NULL) {
                tmp = (*ptrht)[i];

                if (tmp->data.isFunct){
                    if (tmp->data.parameters != NULL) {
                        DisposeList(tmp->data.parameters);
                        free(tmp->data.parameters);
                    }
                }

                (*ptrht)[i] = (*ptrht)[i]->ptrnext;
                free(tmp);
            }
        }
    }
}

void htClearLocal ( tHTable* ptrht){
    if (ptrht != NULL) {
        tHTItem *tmp;
        for (int i = 0; i < HTSIZE; i++) {
            while ((*ptrht)[i] != NULL) {
                tmp = (*ptrht)[i];

                if (tmp->data.isParam) {
                    tmp->data.type = dtUndefined;
                    break;
                } else {
                    (*ptrht)[i] = (*ptrht)[i]->ptrnext;
                    free(tmp);
                }
            }
        }
    }
}
