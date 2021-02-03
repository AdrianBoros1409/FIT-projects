/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Dynamic string implementation
 * @file string.c
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "string.h"
#include <stdbool.h>

#define STR_LEN_INC 8

#define STR_ERROR   1
#define STR_SUCCESS 0

int strInit(string *s)
{
    if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
        return STR_ERROR;
    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = STR_LEN_INC;
    return STR_SUCCESS;
}

void strFree(string *s)
{
    free(s->str);
}

void strClear(string *s)
{
    s->str[0] = '\0';
    s->length = 0;
}

int strAddChar(string *s1, char c)
{
    if (s1->length + 1 >= s1->allocSize)
    {
        if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
            return STR_ERROR;
        s1->allocSize = s1->length + STR_LEN_INC;
    }
    s1->str[s1->length] = c;
    s1->length++;
    s1->str[s1->length] = '\0';
    return STR_SUCCESS;
}

int strCmpConstStr(string *s1, char* s2)
{
    return strcmp(s1->str, s2);
}

int *string_concat(string *s1, const char* s2)
{
    int length_const_char = strlen(s2);
    if((s1->length + length_const_char + 1) >= s1->allocSize){
        s1->str = (char*)realloc(s1->str,s1->length + length_const_char + 1);
    }
    strncpy(&s1->str[s1->length], s2, length_const_char);
    s1->str[s1->length+ length_const_char] = '\0';
    s1->allocSize = s1->length + length_const_char;
    s1->length += length_const_char;
    return STR_SUCCESS;
}

int strGetLength(string *s)
{
    return s->length;
}

