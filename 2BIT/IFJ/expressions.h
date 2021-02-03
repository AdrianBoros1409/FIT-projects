/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @file expression.h
 * @brief Expression implementation header
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 */

#ifndef IFJ_EXPRESSIONS_H
#define IFJ_EXPRESSIONS_H

#define PREC_TBL_SIZE 8

#include <stdio.h>
#include <stdbool.h>
#include "stack.h"
#include "scanner.h"
#include "parser.h"
#include "scanner.h"

// Expression processing
int checkExpression(parserData* pData);

#endif //IFJ_EXPRESSIONS_H

