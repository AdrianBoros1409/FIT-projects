/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Stack header
 * @file stack.h
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 */


#ifndef IFJ_STACK_H
#define IFJ_STACK_H

#include "symtable.h"
#include <stdbool.h>
#define OK 0
#define ERROR -1

typedef enum
{
	PLUS_S,		//  +
	MINUS_S,	//  -
	MULTI_S,	//  *
	DIV_S,		//  /
	EQ_S,		//  ==
	NEQ_S,		//  !=
	LESS_S,		//  <
	MORE_S,		//  >
	LESS_EQ_S,  //  <=
	MORE_EQ_S,  //  >=
	L_BRAC_S,	//  (
	P_BRAC_S,	//  )
	INT_S,		//  int
	FLOAT_S,	//  float
	STRING_S,	//  string
	ID_S,		//  id
	END_S,		//  $
	FINISH_S,   //  stack bottom
	NON_TERM,	//  nonterm
	BRACKETS,
	SYM_OK,
	SYM_ERROR
} tPrecTblSym;

// Stack item structure
typedef struct item{
	tPrecTblSym sym;
    dType type;
    struct item *nex_ptr;
} Stack_item;

// Stack structure
typedef struct Stack {
    Stack_item *top;
}Stack;

// Function initializes stack
int Stack_init(Stack *stack);

// Function pushes symbol and assigns its type
int Stack_push(Stack *stack, tPrecTblSym sym, dType type);

// Function pushes item after FINISH_S symbol
bool Stack_push_finish(Stack* stack);

// Function returns the first item after FINISH_S symbol
Stack_item *Stack_top_after_finish(Stack* stack);

// Function returns the number of items before FINISH_S symbol
int Stack_count_to_finish(Stack* stack);

// Function pops item on top
void *Stack_pop(Stack *stack);

// Functiion frees all resources
void Stack_dispose(Stack *stack);

// Function return true when stack is empty
bool Stack_empty(Stack *stack);

#endif
