/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Scanner header
 * @file scanner.h
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Matouš Sloboda <xslobo04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#include <stdio.h>

#include "symtable.h"
#define START 0

#define LEXICAL_ERROR 1

#define ID 2
#define NUM 3
#define KEY_WORD 4
#define ASSIGNMENT 5
#define PLUS 6
#define MINUS 7
#define MULTIPLY 8
#define DIVIDE 9
#define NUM_FLOAT 10
#define NOT_EQUAL 11
#define ROUND_BRACKET_LEFT 12
#define ROUND_BRACKET_RIGHT 13
#define EOL 14
#define EQUAL 15
#define COMMA 16
#define END_OF_FILE 18
#define MORE_THAN 19
#define MORE_THAN_EQUAL 20
#define LESS_THAN 21
#define LESS_THAN_EQUAL 22
#define STRING_START 23
#define STRING 24
#define NUM_EXPONENT 25
#define NUM_EXPONENT_NUM 26
#define NUM_EXPONENT_SIGN 27
#define BLOCK_COMMENT 28
#define COMMENT_IGNORE 29
#define ESCAPE_SEQUENCE 30
#define NEGATE 31
#define LINE_COMMENT 32
#define NUM_POINT_FLOAT 33

#define KEYWORD_SUM 17

typedef struct  {
    string* str;
    int number;
    double numberD;
} Attribute;

typedef struct {
    int type;
    Attribute atr;
} Token;

void setSource(FILE *f);

Token scan();

#endif //IFJ_SCANNER_H
