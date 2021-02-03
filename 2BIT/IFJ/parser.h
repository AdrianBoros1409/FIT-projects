/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Parser header
 * @brief Tím 112 / varianta II
 * @file parser.h
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

#define SYNTAX_OK       0
#define SYNTAX_ERROR    2
#define SEM_ERROR       3
#define SEM_ERROR_TYPE  4
#define SEM_ERROR_PARAM 5
#define SEM_ERROR_OTHER 6
#define INTERNAL_ERROR  99

#include <stdio.h>
#include "scanner.h"
#include "generator.h"

// Parser data structure
typedef struct {
    tHTable *locTable;
    tHTable *globTable;
    Token tkn;
    tKey functionName;
    tKey idName;
    tKey retType;
    bool allocStatus;
    bool inDef;
    bool inPrint;
    bool isAssign;
    bool inIfWhile;
    bool declaredInIf;
    int paramCount;
    int result;
    int ifCounter;
    int whileCounter;
    int CounterW;
    int Counter;
    tData *currID;
} parserData;

// Lexical and semantic analysis of the code
int parse(tHTable *lsTable, tHTable  *gsTable);

// Function searches for invalid characters
bool checkInvalidChar(tKey temp);

#endif //IFJ_PARSER_H
