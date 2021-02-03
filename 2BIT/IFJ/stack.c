/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Stack implementation
 * @file stack.c
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 */

#include "stack.h"

int Stack_init(Stack *stack)
{
    if (!stack)
        return ERROR;
    stack->top = NULL;
    return OK;
}

bool Stack_empty(Stack *stack)
{
    return(stack->top == NULL);
}

int Stack_push(Stack *stack, tPrecTblSym sym, dType type)
{
    Stack_item* new_push_item = (Stack_item*)malloc(sizeof(Stack_item));
    if(new_push_item != NULL)
    {
        new_push_item->nex_ptr = stack->top;
        new_push_item->sym = sym;
        new_push_item->type = type;
        stack->top = new_push_item;
        return OK;
    }
    return ERROR;
}

bool Stack_push_finish(Stack* stack){
    Stack_item *temp = stack->top;
    Stack_item *prev = NULL;

    while(temp != NULL){
        if (temp->sym != FINISH_S && temp->sym != NON_TERM) {
            Stack_item *new = (Stack_item*)malloc(sizeof(Stack_item));
            if (new != NULL) {
                new->sym = FINISH_S;
                new->type = dtUndefined;

                if (prev != NULL) {
                    new->nex_ptr = prev->nex_ptr;
                    prev->nex_ptr = new;
                } else {
                    new->nex_ptr = stack->top;
                    stack->top = new;
                }
                return true;
            } else {
                return false;
            }
        }
        prev = temp;
        temp = temp->nex_ptr;
    }
    return false;
}

Stack_item *Stack_top_after_finish(Stack* stack){
    Stack_item *temp = stack->top;

    while(temp != NULL){
        if (temp->sym != FINISH_S && temp->sym != NON_TERM) {
            return temp;
        } else {
            temp = temp->nex_ptr;
        }
    }
    return NULL;
}

int Stack_count_to_finish(Stack* stack){
    int count = 0;

    Stack_item* isFinish = stack->top;
    while (isFinish != NULL) {
        if (isFinish->sym != FINISH_S) {
            count++;
        } else {
            return count;
        }
        isFinish = isFinish->nex_ptr;
    }
    return ERROR;
}

void *Stack_pop(Stack *stack) {
    Stack_item *temp = stack->top;
    stack->top = temp->nex_ptr;
    free(temp);
    return NULL;
}

void Stack_dispose(Stack *stack)
{
    if (!(Stack_empty(stack)))
    {
        while(stack->top != NULL)
        {
            Stack_pop(stack);
        }
        stack->top = NULL;
    }
}
