/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Parser implementation
 * @brief Tím 112 / varianta II
 * @file parser.c
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#include "parser.h"
#include "expressions.h"

parserData pData;

int prog();
int defValue();
int command();
int arg();

bool checkInvalidChar(tKey temp)
{
    const char *invalid_chars = "?!";
    while (*temp)
    {
        if(strchr(invalid_chars, *temp))
            return true;
        temp ++;
    }
    return false;
}

int eolToCommand() {
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }

    // EOL <command>
    if (pData.tkn.type == EOL) {
        pData.tkn = scan();
        if (pData.tkn.type == LEXICAL_ERROR) {
            return pData.result = LEXICAL_ERROR;
        }
        pData.result = command();
    } else {
        pData.result = SYNTAX_ERROR;
    }
    return pData.result;
}

int ifStatement(){
    Gen_if_start_comment(pData.ifCounter,pData.inDef);
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }

    //<command> -> if <expression>
    pData.result = checkExpression(&pData);
    if (pData.result != SYNTAX_OK) {
        return pData.result;
    }
    tKey id = "if_false";
    tKey idEnd = "if_end";
    pData.inIfWhile = true;
    int Count = pData.Counter;
    pData.Counter ++;
    Gen_if_start(id, pData.ifCounter, Count, pData.inDef);
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }

    //<command> -> if <expression> then
    if (strCmpConstStr(pData.tkn.atr.str, "then") == 0) {
        pData.declaredInIf = true;

        //<command> -> if <expression> then EOL <command>
        pData.result = eolToCommand();
        if (pData.result != SYNTAX_OK) {
            return pData.result;
        }
        Gen_jump(idEnd, pData.ifCounter, Count, pData.inDef);
        Gen_if_else(id, pData.ifCounter, Count, pData.inDef);

        //<command> -> if <expression> then EOL
        if (pData.tkn.type == EOL) {
            pData.tkn = scan();
            if (pData.tkn.type == LEXICAL_ERROR) {
                return pData.result = LEXICAL_ERROR;
            }

            //<command> -> if <expression> then EOL <command>
            pData.result = command();
            if (pData.result != SYNTAX_OK) {
                return pData.result;
            }
        }

        //<command> -> if <expression> then EOL <command> else
        if (strCmpConstStr(pData.tkn.atr.str, "else") == 0) {
            pData.declaredInIf = false;

            //<command> -> if <expression> then EOL <command> else EOL <command>
            pData.result = eolToCommand();
            if (pData.result != SYNTAX_OK) {
                return pData.result;
            }
            if (pData.tkn.type == EOL) {
                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }

                //<command> -> if <expression> then EOL <command> else EOL <command>
                pData.result = command();
                if (pData.result != SYNTAX_OK) {
                    return pData.result;
                }
            }
            Gen_if_label(idEnd, pData.ifCounter, Count, pData.inDef);

            //<command> -> if <expression> then EOL <command> else EOL <command> end
            if (strCmpConstStr(pData.tkn.atr.str, "end") == 0) {

                //<command> -> if <expression> then EOL <command> else EOL <command> end EOL <command>
                pData.result = eolToCommand();
                if (pData.result != SYNTAX_OK) {
                    return pData.result;
                }
            } else {
                pData.result = SYNTAX_ERROR;
            }
        } else {
            pData.result = SYNTAX_ERROR;
        }
    } else {
        pData.result = SYNTAX_ERROR;
    }
    pData.ifCounter --;
    if (Count == pData.Counter) {
        pData.inIfWhile = false;
    }
    return pData.result;
}

int whileLoop(){
    Gen_while_comment(pData.whileCounter, pData.inDef);
    Gen_while_label("while", pData.whileCounter, pData.CounterW, pData.inDef);
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }

    //<comman> -> while <expression>
    pData.result = checkExpression(&pData);
    if (pData.result != SYNTAX_OK) {
        return pData.result;
    }
    pData.inIfWhile = true;
    tKey whileId = "while";
    tKey whileEnd = "while_end";
    int whileCount = pData.CounterW;
    pData.CounterW ++;
    Gen_while_start(whileEnd, pData.whileCounter, whileCount, pData.inDef);
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }

    //<command> -> while <expression> do
    if (strCmpConstStr(pData.tkn.atr.str, "do") == 0) {
        //<command> -> while <expression> do EOL <command>
        pData.result = eolToCommand();
        if (pData.result != SYNTAX_OK) {
            return pData.result;
        }
        if (pData.tkn.type == EOL) {
            pData.tkn = scan();
            if (pData.tkn.type == LEXICAL_ERROR) {
                return pData.result = LEXICAL_ERROR;
            }
            //<command> -> while <expression> do EOL <command>
            pData.result = command();
            if (pData.result != SYNTAX_OK) {
                return pData.result;
            }
        }
        Gen_while_end(whileId, pData.whileCounter, whileCount, pData.inDef);
        Gen_while_label(whileEnd, pData.whileCounter, whileCount, pData.inDef);

        //<command> -> while <expression> do EOL <command> end
        if (strCmpConstStr(pData.tkn.atr.str, "end") == 0) {
            //<command> -> while <expression> do EOL <command> end EOL <command>
            pData.result = eolToCommand();
        } else {
            pData.result = SYNTAX_ERROR;
        }
    } else {
        pData.result = SYNTAX_ERROR;
    }
    pData.whileCounter--;
    if (whileCount == pData.whileCounter) {
        pData.inIfWhile = false;
    }
    return pData.result;
}

int print(){
    pData.retType = "nil";
    pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
    if (pData.allocStatus == false) {
        return pData.result = INTERNAL_ERROR;
    }
    pData.currID->isFunct = true;
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }
    pData.inPrint = true;

    //def_value> -> print <arg>
    pData.result = arg();
    pData.inPrint = false;
    if (pData.tkn.type == EOL && pData.result == SYNTAX_OK) {
        pData.tkn = scan();
        if (pData.tkn.type == LEXICAL_ERROR) {
            return pData.result = LEXICAL_ERROR;
        }
        pData.result = command();
    }
    return pData.result;
}

int param() {
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }
    if (pData.tkn.type == COMMA) {
        pData.tkn = scan();
        if (pData.tkn.type == LEXICAL_ERROR) {
            return pData.result = LEXICAL_ERROR;
        }
        if (pData.tkn.type == ID) {
            if (strcmp(pData.currID->identifier, pData.tkn.atr.str->str) == 0)
                return pData.result = SEM_ERROR;
            htInsert(pData.locTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
            if (pData.allocStatus == false) {
                pData.result = INTERNAL_ERROR;
            } else {
                if (PostInsert(pData.currID->parameters, pData.tkn.atr.str->str) == INTERNAL_ERROR) {
                    return pData.result = INTERNAL_ERROR;
                }
                htRead(pData.locTable, pData.tkn.atr.str->str)->isParam = true;
                Gen_var_declaration(pData.tkn.atr.str->str, pData.inDef);
                pData.paramCount++;
                Gen_pass_param(pData.tkn.atr.str->str, pData.paramCount, pData.inDef);

                //<param> -> , id <param>
                pData.result = param();
            }
        } else {
            pData.result = SYNTAX_ERROR;
        }
    }
    pData.paramCount = 0;
    return pData.result;
}

int paramList() {
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }
    if (pData.tkn.type == ID) {
        if (strcmp(pData.currID->identifier, pData.tkn.atr.str->str) == 0)
            return pData.result = SEM_ERROR;

        htInsert(pData.locTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
        if (pData.allocStatus == false) {
            pData.result = INTERNAL_ERROR;
        } else {
            if (InsertFirst(pData.currID->parameters, pData.tkn.atr.str->str) == INTERNAL_ERROR) {
                return pData.result = INTERNAL_ERROR;
            }
            htRead(pData.locTable, pData.tkn.atr.str->str)->isParam = true;
            Gen_var_declaration(pData.tkn.atr.str->str, pData.inDef);
            Gen_pass_param(pData.tkn.atr.str->str, pData.paramCount, pData.inDef);

            //<param_list> -> id <param>
            pData.result = param();
        }
    }
    return pData.result;
}

int defID(){
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }
    pData.inDef=true;
    if (pData.tkn.type == ID) {
        if (htSearch(pData.globTable, pData.tkn.atr.str->str) == NULL){
            pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
            pData.currID->isFunct = true;
            Gen_function_start(pData.currID->identifier);
            Gen_function_defvar();
            pData.functionName = pData.currID->identifier;
            tList* paramList = malloc(sizeof(tList));
            InitList(paramList);
            pData.currID->parameters = paramList;
            //generuj zaciatok funkcie
        } else {
            return pData.result = SEM_ERROR;
        }
    } else {
        return pData.result = SYNTAX_ERROR;
    }
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }
    if (pData.tkn.type == ROUND_BRACKET_LEFT) {
        //<def_id> -> def FunctId (<param_list>)
        pData.result = paramList();
        if (pData.result != SYNTAX_OK)
            return pData.result;

        if (pData.tkn.type == ROUND_BRACKET_RIGHT && pData.result == SYNTAX_OK) {
            ListFirst(pData.currID->parameters);

            pData.tkn = scan();
            if (pData.tkn.type == LEXICAL_ERROR) {
                return pData.result = LEXICAL_ERROR;
            }

            if (pData.tkn.type == EOL) {
                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }

                //<def_id> -> def FunctId (<param_list>) EOL <command>
                pData.result = command();
                if(pData.result != SYNTAX_OK)
                    return pData.result;

                if (pData.tkn.type == EOL) {
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                }
                if (strCmpConstStr(pData.tkn.atr.str, "end") == 0) {
                    if (pData.idName != NULL && strcmp(pData.retType, "id") == 0 )
                    {
                        Gen_store_id(pData.idName);
                        Gen_function_return();
                    }
                    else if (strcmp(pData.retType, "value") == 0)
                    {
                        Gen_store_value();
                        Gen_function_return();
                    }
                    pData.inDef = false;
                    pData.retType = "nil";
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                    if (pData.tkn.type != EOL)
                        return pData.result = SYNTAX_ERROR;
                    Gen_function_end(pData.functionName);

                    htClearLocal(pData.locTable);
                    //<def_id> -> def FunctId (<param_list>) EOL <command> end <prog>
                    pData.result = prog();
                } else {
                    pData.result = SYNTAX_ERROR;
                }
            } else {
                pData.result = SYNTAX_ERROR;
            }
        } else {
            pData.result = SYNTAX_ERROR;
        }
    } else {
        pData.result = SYNTAX_ERROR;
    }
    return pData.result;
}

int value(){
    tData* currToken = NULL;
    switch (pData.tkn.type){
        // <value> -> int_value
        case NUM:
            if (pData.currID != NULL) {
                if (pData.currID->type != dtUndefined && pData.currID->type != dtInt) {
                    pData.result = SEM_ERROR;
                }
                if (pData.currID->isFunct) {
                    if (strcmp(pData.currID->identifier, "print") == 0) {
                        return pData.result = SYNTAX_OK;
                    }
                    if (pData.currID->parameters->Act != NULL) {
                        if (((strcmp(pData.currID->identifier, "substr") == 0) ||
                             (strcmp(pData.currID->identifier, "length") == 0) ||
                             (strcmp(pData.currID->identifier, "ord") == 0)    ||
                             (strcmp(pData.currID->identifier, "chr") == 0)) && (htRead(pData.locTable, pData.currID->parameters->Act->param)->type != dtInt)) {
                                return pData.result = SEM_ERROR_TYPE;
                        } else {
                            if ((strcmp(pData.currID->identifier, "chr") == 0) && (pData.tkn.atr.number > 255 || pData.tkn.atr.number < 0)) {
                                return pData.result = SEM_ERROR_TYPE;
                            }
                            htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtInt;
                        }
                        ListSucc(pData.currID->parameters);
                    } else {
                        return pData.result = SEM_ERROR_PARAM;
                    }
                } else {
                    pData.currID->type = dtInt;
                }
            }
            break;
        // <value> -> float_value
        case NUM_FLOAT:
            if (pData.currID != NULL) {
                if (pData.currID->type != dtUndefined && pData.currID->type != dtFloat) {
                    pData.result = SEM_ERROR;
                }
                if (pData.currID->isFunct) {
                    if (strcmp(pData.currID->identifier, "print") == 0) {
                        return pData.result = SYNTAX_OK;
                    }
                    if (pData.currID->parameters->Act != NULL) {
                        if (strcmp(pData.currID->identifier, "length") == 0) {
                            return pData.result = SEM_ERROR_TYPE;
                        } else if (strcmp(pData.currID->identifier, "substr") == 0 || strcmp(pData.currID->identifier, "ord") == 0) {
                            tData* substrRead = htRead(pData.locTable, pData.currID->parameters->Act->param);
                            if (substrRead->type == dtInt) {
                                htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtInt;
                            } else {
                                return  pData.result = SEM_ERROR_TYPE;
                            }
                        } else if (strcmp(pData.currID->identifier, "chr") == 0) {
                            htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtInt;
                        } else {
                            htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtFloat;
                        }
                        ListSucc(pData.currID->parameters);
                    } else {
                        return pData.result = SEM_ERROR_PARAM;
                    }
                } else {
                    pData.currID->type = dtFloat;
                }
            }
            break;
        // <value> -> string_value
        case STRING:
            if (pData.currID != NULL) {
                if (pData.currID->type != dtUndefined && pData.currID->type != dtString) {
                    pData.result = SEM_ERROR;
                }
                if (pData.currID->isFunct) {
                    if (strcmp(pData.currID->identifier, "print") == 0) {
                        return pData.result = SYNTAX_OK;
                    }
                    if (pData.currID->parameters->Act != NULL) {
                        if (((strcmp(pData.currID->identifier, "length") == 0) ||
                             (strcmp(pData.currID->identifier, "ord") == 0)    ||
                             (strcmp(pData.currID->identifier, "chr") == 0)    ||
                             (strcmp(pData.currID->identifier, "substr") == 0)) && (htRead(pData.locTable, pData.currID->parameters->Act->param)->type != dtString)) {
                                return pData.result = SEM_ERROR_TYPE;
                        } else {
                            htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtString;
                        }
                        ListSucc(pData.currID->parameters);
                    } else {
                        return pData.result = SEM_ERROR_PARAM;
                    }
                } else {
                    pData.currID->type = dtString;
                }
            }
            break;
        // <value> -> id
        case ID:
            if (htSearch(pData.globTable, pData.tkn.atr.str->str) == NULL) {
                if ((htSearch(pData.locTable, pData.tkn.atr.str->str) != NULL) && pData.inDef ) {
                    currToken = htRead(pData.locTable, pData.tkn.atr.str->str);
                } else {
                    return pData.result = SEM_ERROR;
                }
            } else {
                currToken = htRead(pData.globTable, pData.tkn.atr.str->str);
            }
            if (pData.currID->isFunct) {
                if (strcmp(pData.currID->identifier, "print") == 0) {
                    return pData.result = SYNTAX_OK;
                }
                if (pData.currID->parameters->Act != NULL) {
                    if (strcmp(pData.currID->identifier, "substr") == 0 || strcmp(pData.currID->identifier, "ord") == 0) {
                        tData* substrRead = htRead(pData.locTable, pData.currID->parameters->Act->param);
                        if (substrRead->type == dtInt) {
                            if (currToken->type != dtInt && currToken->type != dtFloat) {
                                return pData.result = SEM_ERROR_TYPE;
                            } else {
                                htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtInt;
                            }
                        } else {
                            if (htRead(pData.locTable, pData.currID->parameters->Act->param)->type != currToken->type) {
                                return pData.result = SEM_ERROR_TYPE;
                            }
                        }
                    } else if (strcmp(pData.currID->identifier, "chr") == 0) {
                        if (currToken->type == dtInt || currToken->type == dtFloat) {
                            htRead(pData.locTable, pData.currID->parameters->Act->param)->type = dtInt;
                        } else {
                            return pData.result = SEM_ERROR_TYPE;
                        }
                    } else if (strcmp(pData.currID->identifier, "length") == 0) {
                        if (htRead(pData.locTable, pData.currID->parameters->Act->param)->type != currToken->type) {
                            return pData.result = SEM_ERROR_TYPE;
                        }
                    } else {
                        htRead(pData.locTable, pData.currID->parameters->Act->param)->type = currToken->type;
                    }
                    ListSucc(pData.currID->parameters);
                } else {
                    return pData.result = SEM_ERROR_PARAM;
                }
            } else {
                pData.currID->type = currToken->type;
                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }
            }
            break;
        default:
            pData.result = SYNTAX_ERROR;
            break;
    }
    return pData.result;
}

int argN() {
    pData.result = SYNTAX_OK;
    pData.tkn = scan();
    if (pData.tkn.type == LEXICAL_ERROR) {
        return pData.result = LEXICAL_ERROR;
    }
    if (pData.tkn.type == COMMA) {
        pData.tkn = scan();
        if (pData.tkn.type == LEXICAL_ERROR) {
            return pData.result = LEXICAL_ERROR;
        }
        // <arg_n> -> , <value>
        pData.result = value();
        if (pData.result == SYNTAX_OK) {
            if (pData.inPrint) {
                Gen_pushs(pData.inDef, pData.tkn);
                Gen_print_pops(pData.inDef);
                Gen_function_print(pData.inDef);
            }
            else {
                Gen_temp_stack(pData.tkn, pData.paramCount, pData.inDef);
                pData.paramCount++;
            }
            // <arg_n> -> , <value> <arg_n>
            pData.result = argN();
        } else {
            return pData.result;
        }
    }
    else if(pData.tkn.type != EOL && pData.tkn.type != ROUND_BRACKET_RIGHT )
        return pData.result = SYNTAX_ERROR;
    else
        if (strcmp(pData.currID->identifier, "print") != 0) {
            if ((pData.currID->isFunct) && (pData.currID->parameters->Act != NULL)) {
                return pData.result = SEM_ERROR_PARAM;
            }
        }
    return pData.result;
}

int argN2() {
    if (pData.tkn.type == ROUND_BRACKET_RIGHT && pData.currID->isFunct) {
        if (pData.inPrint == true)
        {
            return pData.result = SYNTAX_ERROR;
        }
        else if (pData.currID->parameters->First != NULL) {
            return pData.result = SEM_ERROR_PARAM;
        } else {
            return pData.result = SYNTAX_OK;
        }
    } else {
        //<arg_n2> -> <value>
        pData.result = value();
        if (pData.result == SYNTAX_OK) {
            if (pData.inPrint) {
                Gen_pushs(pData.inDef, pData.tkn);
                Gen_print_pops(pData.inDef);
                Gen_function_print(pData.inDef);
            }
            //<arg_n2> -> <value> <arg_n>
            pData.result = argN();
        } else {
            return pData.result;
        }
        return pData.result;
    }
}

int arg(){
    pData.result = SYNTAX_OK;
    Gen_frame(pData.inDef);
    if (pData.tkn.type == ROUND_BRACKET_LEFT) {
        pData.tkn = scan();

        if (pData.tkn.type == LEXICAL_ERROR) {
            return pData.result = LEXICAL_ERROR;
        }
        if (pData.tkn.type != ROUND_BRACKET_RIGHT && pData.inPrint == false) {
            Gen_temp_stack(pData.tkn, pData.paramCount, pData.inDef);
            pData.paramCount++;
        }
        //<arg> -> (<arg_n2>)
        pData.result = argN2();
        if (pData.result != SYNTAX_OK) {
            return pData.result;
        }
        if (pData.tkn.type == ROUND_BRACKET_RIGHT) {
            if (pData.currID->parameters != NULL) {
                ListFirst(pData.currID->parameters);
            }
            pData.tkn = scan();
            if (pData.tkn.type == LEXICAL_ERROR) {
                return pData.result = LEXICAL_ERROR;
            }
            pData.result = SYNTAX_OK;
        } else {
            pData.result = SYNTAX_ERROR;
        }
    }
    else {
        if(pData.inPrint == false && (pData.currID->parameters->Count > 0 && pData.tkn.type == EOL))
            return pData.result = SEM_ERROR_PARAM;

        //<arg> -> <value>
        pData.result = value();
        if (pData.inPrint) {
            Gen_pushs(pData.inDef, pData.tkn);
            Gen_print_pops(pData.inDef);
            Gen_function_print(pData.inDef);
        }
        else {
            Gen_temp_stack(pData.tkn, pData.paramCount, pData.inDef);
            pData.paramCount++;
        }
        if (pData.result == SYNTAX_OK) {
            //<arg> -> <value> <arg_n>
            pData.result = argN();
        }
    }
    pData.paramCount = 0;
    if (pData.currID->parameters != NULL) {
        ListFirst(pData.currID->parameters);
    }
    return pData.result;
}

int defValue(){
    pData.result = SYNTAX_OK;
    switch(pData.tkn.type){
        case ID:
            if ((htSearch(pData.locTable, pData.tkn.atr.str->str)) != NULL) {
                //<def_value> -> <expression>
                pData.result = checkExpression(&pData);
                if (pData.result != SYNTAX_OK) {
                    return pData.result;
                }
            } else
                if (htSearch(pData.globTable, pData.tkn.atr.str->str) != NULL) {
                    if (htSearch(pData.globTable, pData.tkn.atr.str->str)->data.isFunct) {
                        pData.currID = htRead(pData.globTable, pData.tkn.atr.str->str);
                        pData.tkn = scan();
                        if (pData.tkn.type == LEXICAL_ERROR) {
                            return pData.result = LEXICAL_ERROR;
                        }
                        if(pData.currID->parameters->Count != 0 || pData.tkn.type == ROUND_BRACKET_LEFT) {
                            //<def_value> -> FunctID <arg>
                            pData.result = arg();
                        } else if (pData.currID->parameters->Count == 0 && pData.tkn.type != EOL) {
                            if (pData.tkn.type != NUM && pData.tkn.type != ID && pData.tkn.type != STRING && pData.tkn.type != NUM_FLOAT)
                                return pData.result = SYNTAX_ERROR;
                            return pData.result = SEM_ERROR_PARAM;
                        }
                    } else
                        if (pData.inDef) {
                            pData.result = SEM_ERROR;
                        } else {
                            //<def_value> -> <expression>
                            pData.result = checkExpression(&pData);
                            if (pData.result != SYNTAX_OK) {
                                return pData.result;
                            }
                        }
                } else {
                    return pData.result = SEM_ERROR;
                }
            if(pData.currID != NULL && pData.currID->isFunct == true) {
                Gen_call(pData.currID->identifier, pData.inDef);
                if (pData.isAssign == true) {
                      Gen_check_type(pData.idName, pData.inDef);
                }
            }
            break;
        case NUM:
        case NUM_FLOAT:
        case STRING:
            //<def_value> -> <expression>
            pData.result = checkExpression(&pData);
            if (pData.result != SYNTAX_OK) {
                return pData.result;
            }
            break;
        case KEY_WORD:
            if (strCmpConstStr(pData.tkn.atr.str, "nil") == 0 && pData.isAssign) {
                if (pData.currID->type != dtUndefined) {
                    return pData.result = SEM_ERROR;
                } else {
                    pData.currID->type = dtUndefined;
                    pData.tkn = scan();
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "nil") == 0) {
                pData.tkn = scan();
                if (pData.tkn.type == ASSIGNMENT)
                    return pData.result = SYNTAX_ERROR;
            } else if (strCmpConstStr(pData.tkn.atr.str, "length") == 0) {
                bool int2float =false;
                if (pData.currID != NULL && pData.isAssign)
                {
                    if (pData.currID->type == dtInt || pData.currID->type == dtUndefined)
                        pData.currID->type = dtInt;
                    else if (pData.currID->type == dtFloat)
                        //pretypuj int2float
                        int2float = true;
                    else
                        return pData.result = SEM_ERROR;

                }
                pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.currID->isFunct = true;
                tList* paramList = malloc(sizeof(tList));
                InitList(paramList);
                pData.currID->parameters = paramList;
                InsertFirst(pData.currID->parameters, "_sStringLength");

                htInsert(pData.locTable, "_sStringLength", &(pData.allocStatus), pData.declaredInIf)->type = dtString;
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }
                // <def_value> -> length <arg>
                pData.result = arg();
                Gen_call(pData.currID->identifier, pData.inDef);
                if (pData.isAssign == true) {
                    if (int2float)
                        Gen_int2float(pData.inDef);
                    Gen_function_retval(pData.idName, pData.inDef);
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "print") == 0) {
                pData.result = print();
            } else if (strCmpConstStr(pData.tkn.atr.str, "inputs") == 0) {
                if (pData.currID != NULL && (pData.currID->type == dtFloat || pData.currID->type == dtInt))
                    return pData.result = SEM_ERROR;
                Gen_inputs(pData.inDef);
                if (pData.isAssign) {
                    Gen_input_store(pData.idName, pData.inDef);
                    pData.currID->type = dtString;
                }
                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }

                if (pData.tkn.type == ROUND_BRACKET_LEFT) {
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                    if (pData.tkn.type != ROUND_BRACKET_RIGHT) {
                        return pData.result = SYNTAX_ERROR;
                    } else {
                        pData.tkn = scan();
                        if (pData.tkn.type == LEXICAL_ERROR) {
                            return pData.result = LEXICAL_ERROR;
                        }
                    }
                }
                if (pData.tkn.type != EOL) {
                    return pData.result = SYNTAX_ERROR;
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "inputi") == 0) {
                if (pData.currID != NULL && pData.currID->type == dtString)
                    return pData.result = SEM_ERROR;
                Gen_inputi(pData.inDef);
                if (pData.isAssign) {
                    if (pData.currID != NULL && pData.currID->type == dtFloat) {
                        Gen_push_input(pData.currID->identifier, pData.inDef);
                        Gen_int2floats(pData.inDef);
                        Gen_input_pop(pData.currID->identifier, pData.inDef);
                    }
                    Gen_input_store(pData.idName, pData.inDef);
                    pData.currID->type = dtInt;
                }
                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }

                if (pData.tkn.type == ROUND_BRACKET_LEFT) {
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                    if (pData.tkn.type != ROUND_BRACKET_RIGHT) {
                        return pData.result = SYNTAX_ERROR;
                    } else {
                        pData.tkn = scan();
                        if (pData.tkn.type == LEXICAL_ERROR) {
                            return pData.result = LEXICAL_ERROR;
                        }
                    }
                }
                if (pData.tkn.type != EOL) {
                    return pData.result = SYNTAX_ERROR;
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "inputf") == 0) {
                if (pData.currID != NULL && pData.currID->type == dtString)
                    return pData.result = SEM_ERROR;
                Gen_inputf(pData.inDef);
                if (pData.isAssign) {
                    if (pData.currID != NULL && pData.currID->type == dtInt) {
                        Gen_push_input(pData.currID->identifier, pData.inDef);
                        Gen_float2ints(pData.inDef);
                        Gen_input_pop(pData.currID->identifier, pData.inDef);
                    }
                    Gen_input_store(pData.idName, pData.inDef);
                    pData.currID->type = dtFloat;
                }
                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }

                if (pData.tkn.type == ROUND_BRACKET_LEFT) {
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                    if (pData.tkn.type != ROUND_BRACKET_RIGHT) {
                        return pData.result = SYNTAX_ERROR;
                    } else {
                        pData.tkn = scan();
                        if (pData.tkn.type == LEXICAL_ERROR) {
                            return pData.result = LEXICAL_ERROR;
                        }
                    }
                }
                if (pData.tkn.type != EOL) {
                    return pData.result = SYNTAX_ERROR;
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "substr") == 0) {
                if (pData.currID != NULL)
                {
                    if (pData.currID->type == dtString || pData.currID->type == dtUndefined)
                        pData.currID->type = dtString;
                    else
                        return pData.result = SEM_ERROR;

                }
                pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.currID->isFunct = true;
                tList* paramList = malloc(sizeof(tList));
                InitList(paramList);
                pData.currID->parameters = paramList;
                InsertFirst(pData.currID->parameters, "_sSubstrString");
                PostInsert(pData.currID->parameters, "_iSubstrBegin");
                PostInsert(pData.currID->parameters, "_nSubstrLength");
                ListFirst(pData.currID->parameters);

                htInsert(pData.locTable, "_sSubstrString", &(pData.allocStatus), pData.declaredInIf)->type = dtString;
                htInsert(pData.locTable, "_iSubstrBegin", &(pData.allocStatus), pData.declaredInIf)->type = dtInt;
                htInsert(pData.locTable, "_nSubstrLength", &(pData.allocStatus), pData.declaredInIf)->type = dtInt;
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }
                // <def_value> -> substr <arg>
                pData.result = arg();
                Gen_call(pData.currID->identifier, pData.inDef);
                if (pData.isAssign == true) {
                    Gen_function_retval(pData.idName, pData.inDef);
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "chr") == 0) {
                pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.currID->isFunct = true;
                tList* paramList = malloc(sizeof(tList));
                InitList(paramList);
                pData.currID->parameters = paramList;
                InsertFirst(pData.currID->parameters, "_iChrAsciiCode");

                htInsert(pData.locTable, "_iChrAsciiCode", &(pData.allocStatus), pData.declaredInIf)->type = dtInt;
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }
                // <def_value> -> chr <arg>
                pData.result = arg();
                Gen_call(pData.currID->identifier, pData.inDef);
                if (pData.isAssign == true) {
                    Gen_function_retval(pData.idName, pData.inDef);
                }
            } else if (strCmpConstStr(pData.tkn.atr.str, "ord") == 0) {
                pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.currID->isFunct = true;
                tList* paramList = malloc(sizeof(tList));
                InitList(paramList);
                pData.currID->parameters = paramList;
                InsertFirst(pData.currID->parameters, "_sOrdString");
                PostInsert(pData.currID->parameters, "_iOrdCharPosition");
                ListFirst(pData.currID->parameters);

                htInsert(pData.locTable, "_sOrdString", &(pData.allocStatus), pData.declaredInIf)->type = dtString;
                htInsert(pData.locTable, "_iOrdCharPosition", &(pData.allocStatus), pData.declaredInIf)->type = dtInt;
                if (pData.allocStatus == false) {
                    return pData.result = INTERNAL_ERROR;
                }

                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                }
                // <def_value> -> ord <arg>
                pData.result = arg();
                Gen_call(pData.currID->identifier, pData.inDef);
                if (pData.isAssign == true) {
                    Gen_function_retval(pData.idName, pData.inDef);
                }
            } else {
                return pData.result = SYNTAX_ERROR;
            }
            break;
        default:
            // <def_value> -> <expression>
            pData.result = checkExpression(&pData);
            if (pData.result != SYNTAX_OK) {
                return pData.result;
            }
            break;
    }

    return pData.result;
}

int command() {
    bool newVar = false;
    switch (pData.tkn.type) {
        case ID:
            if (pData.inDef) {
                pData.retType = "id";
                if (htSearch(pData.locTable, pData.tkn.atr.str->str) == NULL) {
                    if (htSearch(pData.globTable, pData.tkn.atr.str->str) != NULL) {
                        pData.currID = htRead(pData.globTable, pData.tkn.atr.str->str);
                    } else {
                        pData.currID = htInsert(pData.locTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
                        Gen_var_declaration(pData.tkn.atr.str->str, pData.inDef);
                        newVar = true;
                    }
                } else {
                    pData.currID = htRead(pData.locTable, pData.tkn.atr.str->str);
                    if (pData.inIfWhile && (pData.currID->inIf == true && pData.declaredInIf == false)) {
                        Gen_var_declaration(pData.tkn.atr.str->str, pData.inDef);
                        pData.currID->inIf = false;
                    }
                }

                if (pData.allocStatus == false) {
                    pData.result = INTERNAL_ERROR;
                }
                if (pData.currID != NULL) {
                    if (pData.currID->isFunct) {
                        //<command> -> id = <def_value>
                        pData.result = defValue();
                        if (pData.result != SYNTAX_OK) {
                            return pData.result;
                        }
                        if (pData.tkn.type == EOL) {
                            return pData.result = SYNTAX_OK;
                        }
                    }
                }
            } else {
                if (htSearch(pData.globTable, pData.tkn.atr.str->str) == NULL){
                    pData.currID = htInsert(pData.globTable, pData.tkn.atr.str->str, &(pData.allocStatus), pData.declaredInIf);
                    Gen_var_declaration(pData.tkn.atr.str->str, pData.inDef);
                    newVar = true;
                } else {
                    pData.currID = htRead(pData.globTable, pData.tkn.atr.str->str);
                    if (pData.inIfWhile && (pData.currID->inIf == true && pData.declaredInIf == false)) {
                        Gen_var_declaration(pData.tkn.atr.str->str, pData.inDef);
                        pData.currID->inIf = false;
                    }
                }
                if (pData.allocStatus == false) {
                    pData.result = INTERNAL_ERROR;
                }

                if (pData.currID != NULL) {
                    if (pData.currID->isFunct) {
                        //<command> -> id = <def_value>
                        pData.result = defValue();
                        if (pData.result != SYNTAX_OK) {
                            return pData.result;
                        }
                        if (pData.tkn.type == EOL) {
                            return pData.result = SYNTAX_OK;
                        }
                    }
                }
            }
            pData.idName = pData.tkn.atr.str->str;
            if (checkInvalidChar(pData.idName))
                return pData.result = LEXICAL_ERROR;

            pData.tkn = scan();
            if (pData.tkn.type == LEXICAL_ERROR) {
                return pData.result = LEXICAL_ERROR;
            }
            if (pData.tkn.type == COMMA)
                return pData.result = SYNTAX_ERROR;
            if (pData.tkn.type == ASSIGNMENT) {
                pData.isAssign = true;

                pData.tkn = scan();
                if (pData.tkn.type == LEXICAL_ERROR) {
                    return pData.result = LEXICAL_ERROR;
                } else if (pData.tkn.type == EOL) {
                    return pData.result = SYNTAX_ERROR;
                }
                //<command> -> id = <def_value>
                pData.result = defValue();
                if (pData.result != SYNTAX_OK) {
                    return pData.result;
                }
                if (pData.tkn.type == EOL) {
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                    pData.isAssign = false;
                    //<command> -> id = <def_value> EOL <command>
                    pData.result = command();
                } else {
                    pData.result = SYNTAX_ERROR;
                }
            //} else if (pData.tkn.type == EOL && (pData.currID->type == dtUndefined))
            //return pData.result = SEM_ERROR;
            } else if (pData.tkn.type == EOL) {
                if (newVar) {
                    return pData.result = SEM_ERROR;
                } else {
                    pData.tkn = scan();
                    if (pData.tkn.type == LEXICAL_ERROR) {
                        return pData.result = LEXICAL_ERROR;
                    }
                    // //<command> -> id EOL <command>
                    pData.result = command();
                }
            } else if (!pData.currID->isFunct && pData.tkn.type != EOL) {
                return pData.result = SYNTAX_ERROR;
            } else {
                // <command> -> <def_value>
                pData.result = defValue();
                if (pData.result != SYNTAX_OK) {
                    return pData.result;
                }
                //pData.result = SYNTAX_ERROR;
            }
            break;
        case KEY_WORD:
            if (strCmpConstStr(pData.tkn.atr.str, "if") == 0) {
                pData.ifCounter++;
                //<command> -> if
                pData.result = ifStatement();
                if (pData.result != SYNTAX_OK) {
                    return pData.result;
                }
            } else
                if (strCmpConstStr(pData.tkn.atr.str, "while") == 0) {
                    pData.whileCounter++;
                    //<command> -> while
                    pData.result = whileLoop();
                    if (pData.result != SYNTAX_OK) {
                        return pData.result;
                    }
                } else if ((strCmpConstStr(pData.tkn.atr.str, "then") == 0) || (strCmpConstStr(pData.tkn.atr.str, "else") == 0) ||
                           (strCmpConstStr(pData.tkn.atr.str, "end") == 0)  || (strCmpConstStr(pData.tkn.atr.str, "do") == 0)) {
                                if (!pData.inIfWhile && !pData.inDef) {
                                     return pData.result = SYNTAX_ERROR;
                                }
                } else if (strCmpConstStr(pData.tkn.atr.str, "def") == 0) {
                    return pData.result = SYNTAX_OK;
                } else {
                        //<command> -> <def_value>
                        pData.result = defValue();
                }
            break;
        case STRING:
        case NUM:
        case NUM_FLOAT:
        case ROUND_BRACKET_LEFT:
            pData.retType = "value";
            //<command> -> <def_value>
            pData.result = defValue();
            break;
        case MINUS:
        case MULTIPLY:
        case DIVIDE:
            return pData.result = SYNTAX_ERROR;
    }
    return pData.result;
}

int progBody(){
    Gen_main_start();
    // <program_body> -> <command>
    pData.result = command();
    if (pData.result == SYNTAX_OK) {
        // <program_body> -> <command> <prog>
        pData.result = prog();
    }
    return pData.result;
}

int prog(){
    switch (pData.tkn.type) {
        //<prog> -> EOF
        case EOF:
            Gen_main_start();
            return  pData.result;
        //<prog> -> EOL
        case EOL:
            pData.tkn = scan();
            if ( pData.tkn.type == LEXICAL_ERROR) {
                return pData.result = LEXICAL_ERROR;
            }
            pData.result = prog();
            break;
        //<prog> -> <def_id>
        case KEY_WORD:
            if (strCmpConstStr( pData.tkn.atr.str, "def") == 0) {
                // -> <def_id>
                pData.result = defID();
            } else {
                // -> <prog_body>
                pData.result = progBody();
            }
            break;
        // <prog> -> <prog_body>
        default:
            if (pData.result == SYNTAX_OK) {
                pData.result = progBody();
            }
            break;
    }
    return  pData.result;
}

int parse(tHTable *lsTable, tHTable *gsTable) {
    pData.locTable = lsTable;
    pData.globTable = gsTable;
    pData.tkn = scan();
    pData.allocStatus = false;
    pData.isAssign = false;
    pData.inDef = false;
    pData.inPrint = false;
    pData.inIfWhile = false;
    pData.result = SYNTAX_OK;
    pData.currID = NULL;
    pData.ifCounter = 0;
    pData.Counter = 0;
    pData.CounterW = 0;
    pData.whileCounter = 0;
    pData.paramCount = 0;
    pData.retType = "nil";

    if ( pData.tkn.type == LEXICAL_ERROR)
        pData.result = LEXICAL_ERROR;
    else {
        Start_code();
        pData.result = prog();
    }
    if( pData.result == SYNTAX_OK)
        Print_code();

    if (pData.tkn.atr.str != NULL) {

        strFree(pData.tkn.atr.str);
    }

    Free_gen_strings();

    return  pData.result;
}
