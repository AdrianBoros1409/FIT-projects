/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @file expression.c
 * @brief Expression implementation
 * @brief Tím 112 / varianta II
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 */

#include "expressions.h"

// Precedence table as matrix
char prec_tbl[PREC_TBL_SIZE][PREC_TBL_SIZE] = {
/*				   |+-|    |* /|	|R|    |==!=|	|(|		|)|		|i|		|$|*/
/*	|  + - |	*/{	'>',    '<',	'>',	'>',	'<',	'>',	'<',    '>'	},
/*	|  * / |	*/{	'>',	'>',	'>',	'>',	'<',	'>',	'<',    '>'	},
/*	|  R   |	*/{ '<',	'<',	' ',	'>',	'<',	'>',	'<',    '>'	},
/*	| ==!= |	*/{ '<',	'<',	'<',    ' ',	'<',	'>',	'<',	'>'	},
/*	|  (   |	*/{	'<',	'<',	'<',	'<',	'<',	'=',	'<',    ' ' },
/*	|  )   |	*/{ '>',	'>',	'>',	'>',	' ',	'>',	' ',    '>'	},
/*	|  i   |	*/{	'>',    '>',	'>',	'>',	' ',	'>',	' ',	'>'	},
/*	|  $   |	*/{ '<',	'<',	'<',	'<',	'<',	' ',	'<',    ' '	}
};

Stack stack;

// Function returns a symbol according to tkn.type
tPrecTblSym getSym(int type){
    switch(type) {
        case ID:
            return ID_S;
        case NUM:
            return INT_S;
        case PLUS:
            return PLUS_S;
        case MINUS:
            return MINUS_S;
        case MULTIPLY:
            return MULTI_S;
        case DIVIDE:
            return DIV_S;
        case EQUAL:
            return EQ_S;
        case NOT_EQUAL:
            return NEQ_S;
        case LESS_THAN:
            return LESS_S;
        case MORE_THAN:
            return MORE_S;
        case LESS_THAN_EQUAL:
            return LESS_EQ_S;
        case MORE_THAN_EQUAL:
            return MORE_EQ_S;
        case ROUND_BRACKET_LEFT:
            return L_BRAC_S;
        case ROUND_BRACKET_RIGHT:
            return P_BRAC_S;
        case NUM_FLOAT:
            return FLOAT_S;
        case STRING:
            return STRING_S;
        default:
            return END_S;
    }
}

// Function returns the index of the symbol in prec. table
int getPrecIndex(tPrecTblSym sym){
    switch (sym){
        case PLUS_S:
        case MINUS_S:
            return 0;
        case MULTI_S:
        case DIV_S:
            return 1;
        case LESS_S:
        case LESS_EQ_S:
        case MORE_S:
        case MORE_EQ_S:
            return 2;
        case EQ_S:
        case NEQ_S:
            return 3;
        case L_BRAC_S:
            return 4;
        case P_BRAC_S:
            return 5;
        case INT_S:
        case ID_S:
        case FLOAT_S:
        case STRING_S:
            return 6;
        default:
            return 7;
    }
}

// Fuction overrides the type of tkn used in further stack functions
dType convertTypes(parserData* pData){
    switch(pData->tkn.type) {
        case NUM:
            return dtInt;
        case NUM_FLOAT:
            return dtFloat;
        case STRING:
            return dtString;
        case ID:
            if (htSearch(pData->locTable, pData->tkn.atr.str->str) == NULL){
                return dtUndefined;
            } else {
                return htRead(pData->locTable, pData->tkn.atr.str->str)->type;
            }
        default:
            return dtUndefined;
    }
}

tPrecTblSym checkGrammar(parserData* pData, Stack_item* operand1, Stack_item* operand2, Stack_item* operator){
    // Overrides the type of operand to currID.type
    if (operand1->sym == ID_S) {
        operand1->type = pData->currID->type;
        return SYM_OK;

    } else if (operand1->sym == INT_S || operand1->sym == FLOAT_S || operand1->sym == STRING_S ) {
        return SYM_OK;

    // Returns type BRACKETS when the non-term is in brackets
    } else if (operand1->sym == P_BRAC_S && operator->sym == NON_TERM && operand2->sym == L_BRAC_S) {
        return BRACKETS;

    // Returns the operator type when op1 && op2 are non-terms, otherwise -> ERROR
    } else if (operand1->sym == NON_TERM && operand2->sym == NON_TERM) {
        switch (operator->sym) {
            case L_BRAC_S:
            case P_BRAC_S:
            case END_S:
            case FINISH_S:
            case NON_TERM:
                return SYM_ERROR;
            default:
                return operator->sym;
        }
    } else {
        // None of the previous cases were executed -> ERROR
        return SYM_ERROR;
    }
}

// Symbol '>' from the prec. table
int reduce(parserData* pData, Stack* stack, dType* type){
     Stack_item* operand1 = NULL;
     Stack_item* operand2 = NULL;
     Stack_item* operator = NULL;

     // Number of symbols before stack end
     int count = Stack_count_to_finish(stack);
     tPrecTblSym grammarResult;

     switch(count){
         case 1:
             //Processes 1 symbol to non-term, assigns its type
             operand1 = stack->top;
             grammarResult = checkGrammar(pData, operand1, NULL, NULL); //this only assigns op1 type
             if (grammarResult == SYM_ERROR) {
                 Stack_dispose(stack);
                 return pData->result = SYNTAX_ERROR;
             }

             switch(operand1->type){
                 case dtUndefined:
                     *type = operand1->type;
                     break;
                 case dtString:
                     *type = operand1->type;
                     break;
                 case dtFloat:
                     *type = operand1->type;
                     break;
                 case dtInt:
                     *type = operand1->type;
                     break;
             }
             break;
         case 3:
             //Processes 2 operands and the operator
             operand1 = stack->top;
             operator = stack->top->nex_ptr;
             operand2 = stack->top->nex_ptr->nex_ptr;
             grammarResult = checkGrammar(pData, operand1, operand2, operator);
             if (grammarResult == SYM_ERROR) {
                 Stack_dispose(stack);
                 return pData->result = SYNTAX_ERROR;
             }
             break;
         default:
             Stack_dispose(stack);
             return pData->result = SYNTAX_ERROR;
     }

     // Executes statement only when count = 3
     // Generates result according to operator and operand types
     if (grammarResult != SYM_OK) {
             switch (grammarResult) {
                 case BRACKETS:
                     *type = operator->type;
                     break;
                 case MULTI_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         *type = dtInt;
                         Gen_multiply(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         *type = dtFloat;
                         Gen_multiply(pData->inDef);
                     } else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         *type = dtFloat;
                         // INT2FLOAT
                         Gen_int2floats(pData->inDef);
                         Gen_multiply(pData->inDef);
                     }else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         *type = dtFloat;
                         Gen_int2floats_op2(pData->inDef);
                         Gen_multiply(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         *type = dtUndefined;
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_multiply(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     }else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case DIV_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         *type = dtInt;
                         Gen_divide_int(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         *type = dtFloat;
                         Gen_divide(pData->inDef);
                     } else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         *type = dtFloat;
                         // INT2FLOAT
                         Gen_int2floats(pData->inDef);
                         Gen_divide(pData->inDef);
                     }else if(operand1->type == dtFloat && operand2->type == dtInt) {
                         *type = dtFloat;
                         Gen_int2floats_op2(pData->inDef);
                         Gen_divide(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         *type = dtUndefined;
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_divide(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     }else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case MINUS_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         *type = dtInt;
                         Gen_minus(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         *type = dtFloat;
                         Gen_minus(pData->inDef);
                     } else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         *type = dtFloat;
                         // INT2FLOAT
                         Gen_int2floats(pData->inDef);
                         Gen_minus(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         *type = dtUndefined;
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_minus(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     }else if(operand1->type == dtFloat && operand2->type == dtInt) {
                         *type = dtFloat;
                         Gen_float2ints_op2(pData->inDef);
                         Gen_minus(pData->inDef);
                     }else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case PLUS_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         *type = dtInt;
                         Gen_plus(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         *type = dtFloat;
                         Gen_plus(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         *type = dtString;
                         // string concat
                         Gen_concatenate(pData->inDef);
                     } else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         *type = dtFloat;
                         // INT2FLOAT
                         Gen_int2floats(pData->inDef);
                         Gen_plus(pData->inDef);
                     }else if(operand1->type == dtFloat && operand2->type == dtInt) {
                         *type = dtFloat;
                         Gen_int2floats_op2(pData->inDef);
                         Gen_plus(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         *type = dtUndefined;
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_plus(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     }else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case LESS_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         Gen_less(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         Gen_less(pData->inDef);
                     } else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         Gen_int2floats(pData->inDef);
                         Gen_less(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         Gen_int2floats_op2(pData->inDef);
                         Gen_less(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         Gen_less(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_less(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     } else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case MORE_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         Gen_more(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         Gen_more(pData->inDef);
                     } else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         Gen_int2floats(pData->inDef);
                         Gen_more(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         Gen_int2floats_op2(pData->inDef);
                         Gen_more(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         Gen_more(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_more(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     } else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case LESS_EQ_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         Gen_less_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         Gen_less_equal(pData->inDef);
                     }else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         Gen_int2floats(pData->inDef);
                         Gen_less_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         Gen_int2floats_op2(pData->inDef);
                         Gen_less_equal(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         Gen_less_equal(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_less_equal(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     } else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case MORE_EQ_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         Gen_more_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         Gen_more_equal(pData->inDef);
                     }else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         Gen_int2floats(pData->inDef);
                         Gen_more_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         Gen_int2floats_op2(pData->inDef);
                         Gen_more_equal(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         Gen_more_equal(pData->inDef);
                     } else if (operand1->type == dtUndefined || operand2->type == dtUndefined){
                         if (pData->inDef) {
                             Gen_check_2_param_type();
                             Gen_more_equal(pData->inDef);
                         } else {
                             Stack_dispose(stack);
                             return pData->result = SEM_ERROR;
                         }
                     } else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case EQ_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         Gen_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         Gen_equal(pData->inDef);
                     }else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         Gen_int2floats(pData->inDef);
                         Gen_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         Gen_int2floats_op2(pData->inDef);
                         Gen_equal(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         Gen_equal(pData->inDef);
                     } else if ((operand1->type == dtUndefined || operand2->type == dtUndefined) || (operand1->type == dtString || operand2->type == dtString)){
                         if (pData->inDef)
                         {
                             Gen_check_eq_neq_types(true);
                         }
                         else
                            Gen_false(pData->inDef);
                     } else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 case NEQ_S:
                     if (operand1->type == dtInt && operand2->type == dtInt) {
                         Gen_not_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtFloat) {
                         Gen_not_equal(pData->inDef);
                     }else if (operand1->type == dtInt && operand2->type == dtFloat) {
                         Gen_int2floats(pData->inDef);
                         Gen_not_equal(pData->inDef);
                     } else if (operand1->type == dtFloat && operand2->type == dtInt) {
                         Gen_int2floats_op2(pData->inDef);
                         Gen_not_equal(pData->inDef);
                     } else if (operand1->type == dtString && operand2->type == dtString) {
                         Gen_not_equal(pData->inDef);
                     } else if ((operand1->type == dtUndefined || operand2->type == dtUndefined) || (operand1->type == dtString || operand2->type == dtString)){
                         if (pData->inDef)
                         {
                             Gen_check_eq_neq_types(false);
                         }
                         else
                             Gen_false(pData->inDef);
                     } else {
                         Stack_dispose(stack);
                         return pData->result = SEM_ERROR_TYPE;
                     }
                     break;
                 default:
                     break;
             }
     }

     // Exchanges all symbols before the FINISH symbol for one non-term of the correct type
     for (int i = 0; i < count + 1; i++) {
         Stack_pop(stack);
     }
     Stack_push(stack, NON_TERM, *type);

     return pData->result = SYNTAX_OK;
}

int checkExpression(parserData* pData){
    pData->result = SYNTAX_OK;

    // Init stack, mark end of the stack
    Stack_init(&stack);
    Stack_push(&stack, END_S, pData->tkn.type);

    Stack_item* topAfter = Stack_top_after_finish(&stack);
    tPrecTblSym sym = getSym(pData->tkn.type);

    int precIndex1 = getPrecIndex(topAfter->sym);
    int precIndex2 = getPrecIndex(sym);
    char ptIndex = prec_tbl[precIndex1][precIndex2];
    tData* assignToID = pData->currID;
    dType type = dtUndefined;

    bool finish = false;

    // Loops the expression till the end
    while (!finish) {
        switch (ptIndex) {
            case '>':
                pData->result = reduce(pData, &stack, &type);
                if (pData->result != SYNTAX_OK) {
                    return pData->result;
                }
                break;
            case '<':
                if (!Stack_push_finish(&stack)){
                    Stack_dispose(&stack);
                    return pData->result = INTERNAL_ERROR;
                }
                if (Stack_push(&stack, sym, convertTypes(pData)) == ERROR) {
                    Stack_dispose(&stack);
                    return pData->result = INTERNAL_ERROR;
                }
                if (sym == INT_S || sym == FLOAT_S || sym == STRING_S) {
                    //generate
                    Gen_pushs(pData->inDef, pData->tkn);
                    if ((pData->currID != NULL && pData->currID->type == dtUndefined) && (pData->inDef && pData->isAssign))
                        Gen_check_param_type(pData->currID->identifier);
                }
                else if (sym == ID_S) {
                    if (pData->inDef) {
                        pData->currID = htRead(pData->locTable, pData->tkn.atr.str->str);
                        if (pData->currID == NULL) {
                            Stack_dispose(&stack);
                            return pData->result = SEM_ERROR;
                        }
                    } else {
                        pData->currID = htRead(pData->globTable, pData->tkn.atr.str->str);
                        if (pData->currID == NULL) {
                            Stack_dispose(&stack);
                            return pData->result = SEM_ERROR;
                        } else {
                            if (assignToID->type != dtUndefined && pData->currID->type == dtUndefined) {
                                Stack_dispose(&stack);
                                return pData->result = SEM_ERROR;
                            }
                        }
                    }
                    Gen_pushs(pData->inDef,pData->tkn);
                }

                pData->tkn = scan();
                if (pData->tkn.type == LEXICAL_ERROR) {
                    Stack_dispose(&stack);
                    return pData->result = LEXICAL_ERROR;
                }
                break;
            case '=':
                if (Stack_push(&stack, sym, convertTypes(pData)) == ERROR) {
                    Stack_dispose(&stack);
                    return pData->result = INTERNAL_ERROR;
                }

                pData->tkn = scan();
                if (pData->tkn.type == LEXICAL_ERROR) {
                    Stack_dispose(&stack);
                    return pData->result = LEXICAL_ERROR;
                }
                break;
            case ' ':
                if (stack.top->sym != NON_TERM && stack.top->sym != END_S) {
                    Stack_dispose(&stack);
                    return pData->result = SYNTAX_ERROR;
                }
                if (sym == ID_S) {
                    if (pData->inDef) {
                        pData->currID = htRead(pData->locTable, pData->tkn.atr.str->str);
                        if ((pData->currID == NULL) || (pData->currID == assignToID)) {
                            Stack_dispose(&stack);
                            return pData->result = SEM_ERROR;
                        }
                    } else {
                        pData->currID = htRead(pData->globTable, pData->tkn.atr.str->str);
                        if ((pData->currID == NULL) || (pData->currID == assignToID)) {
                            Stack_dispose(&stack);
                            return pData->result = SEM_ERROR;
                        }
                    }
                } else if (sym == P_BRAC_S || (topAfter->sym == L_BRAC_S && sym == END_S)) {
                    return pData->result = SYNTAX_ERROR;
                }
                // End of expression
                if (sym == END_S && topAfter->sym == END_S) {
                    finish = true;
                }
                break;
        }

        if (!finish) {
            topAfter = Stack_top_after_finish(&stack);
            sym = getSym(pData->tkn.type);

            precIndex1 = getPrecIndex(topAfter->sym);
            precIndex2 = getPrecIndex(sym);
            ptIndex = prec_tbl[precIndex1][precIndex2];
        }
    }

    // Generate assignment
    if (pData->isAssign) {
        switch (assignToID->type) {
            case dtUndefined:
                assignToID->type = type;
                Gen_expression_pop(assignToID->identifier, pData->inDef);
                break;
            case dtInt:
                if (type == dtString) {
                    Stack_dispose(&stack);
                    return pData->result = SEM_ERROR;
                } else if (type == dtFloat) {
                    assignToID->type = type;
                    Gen_float2ints(pData->inDef);
                    Gen_expression_pop(assignToID->identifier, pData->inDef);
                } else if (type == dtInt) {
                    assignToID->type = type;
                    Gen_expression_pop(assignToID->identifier, pData->inDef);
                } else if (type == dtUndefined) {
                    assignToID->type = type;
                    Gen_expression_pop(assignToID->identifier, pData->inDef);
                }
                break;
            case dtFloat:
                if (type == dtString) {
                    Stack_dispose(&stack);
                    return pData->result = SEM_ERROR_TYPE;
                } else if (type == dtFloat) {
                    assignToID->type = type;
                    Gen_expression_pop(assignToID->identifier, pData->inDef);
                } else if (type == dtInt) {
                    assignToID->type = type;
                    Gen_int2floats(pData->inDef);
                    Gen_expression_pop(assignToID->identifier, pData->inDef);
                } else if (type == dtUndefined) {
                    assignToID->type = type;
                }
                break;
            case dtString:
                if (type == dtString) {
                    assignToID->type = type;
                    Gen_expression_pop(assignToID->identifier, pData->inDef);
                } else if (type == dtFloat) {
                    Stack_dispose(&stack);
                    return pData->result = SEM_ERROR;
                } else if (type == dtInt) {
                    assignToID->type = type;
                } else if (type == dtUndefined) {
                    assignToID->type = type;
                }
                break;
        }
    }

    Stack_dispose(&stack);
    return pData->result = SYNTAX_OK;
}
