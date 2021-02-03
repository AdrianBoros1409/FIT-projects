/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Dynamic string header
 * @file string.h
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#ifndef IFJ_STRING_H
#define IFJ_STRING_H

// Dynamic string structure
typedef struct
{
    char* str;
    int length;
    int allocSize;
} string;

// Function initializes the string
int strInit(string *s);

// Function frees all resources used by the string
void strFree(string *s);

// Function clears string data and returns it to after-init state
void strClear(string *s);

// Function concatenates string with an array of characters
int *string_concat(string *s1,const char *s2);

//  Function appends a character to the string
int strAddChar(string *s1, char c);

// Function compares a string with and array of characters
int strCmpConstStr(string *s1, char *s2);

// Function returns the length of a string
int strGetLength(string *s);

#endif //IFJ_STRING_H