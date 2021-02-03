/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @file generator.h
 * @brief Code Generator Header
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#ifndef IFJ_GENERATOR_H
#define IFJ_GENERATOR_H


#include "stack.h"
#include "symtable.h"
#include "string.h"
#include "scanner.h"
#include "limits.h"

//Free strings
void Free_gen_strings();

//Generate IFJ18 header
void Start_code();

//Print Generated code
void Print_code();

//Generate start of main
void Gen_main_start();

//Generate end of main
void Gen_main_end();

//Generate Built-in functions
void Gen_built_in_functions();

//Generate start of function
void Gen_function_start(tKey id);

//Generates create frame
void Gen_frame(bool Is_function);

//Generates end of function
void Gen_function_end(tKey id);

//Generates function call
void Gen_call(tKey id, bool Is_function);

//Generates comment If
void Gen_if_start_comment(int cnt, bool Is_function);

//Generates if then
void Gen_if_start(tKey id, int num, int num2, bool Is_function);

//Generates if else
void Gen_if_else(tKey id, int num, int num2, bool Is_function);

//Generates jump
void Gen_jump(tKey id, int num, int num2, bool Is_function);

//Generates label
void Gen_label(tKey id, int num , int num2, bool Is_function);

//Generate if label
void Gen_if_label(tKey id, int num , int num2, bool Is_function);

//Generate while comment
void Gen_while_comment(int cnt, bool Is_function);

//Generates while label
void Gen_while_label(tKey id, int num, int num2, bool Is_function);

//Generates while
void Gen_while_start(tKey id, int num, int num2, bool Is_function);

//Generates while loop
void Gen_while_end(tKey id, int num, int num2, bool Is_function);

//Generates Write
void Gen_function_print(bool Is_function);

//Generates value
void Gen_type_value(Token tkn, bool Is_function);

//Format string
void Format_string(string* format_str, bool Is_function);

//Generates PUSHS
void Gen_pushs(bool Is_function, Token tkn);

//Generate POPS for print
void Gen_print_pops(bool Is_function);

//Generates POPS
void Gen_expression_pop(tKey id, bool Is_function);

//Convert passed parameter type
void Gen_temp_stack(Token tkn, int num, bool Is_function);

//Store value to expression
void Gen_store_value();

//Store id to expression
void Gen_store_id(tKey id);

//Return statement in function
void Gen_function_return();

//Create return variable in function
void Gen_function_defvar();

//Generates conversion int2floats
void Gen_int2floats(bool Is_function);

//Generates conversion float2ints
void Gen_float2ints(bool Is_function);

//Generates conversion float2ints for second operand
void Gen_float2ints_op2(bool Is_function);

//Generates conversion int2floats for second operand
void Gen_int2floats_op2(bool Is_function);

//Generates conversion int2float
void Gen_int2float(bool Is_function);

//Generate variable
void Gen_var_declaration(tKey id, bool Is_function);

//Generate passing parameters
void Gen_pass_param(tKey id, int num, bool Is_function);

//Generates ADDS
void Gen_plus(bool Is_function);

//Generates SUBS
void Gen_minus(bool Is_function);

//Generates DIVS
void Gen_divide(bool Is_function);

//Generates IDIVS
void Gen_divide_int(bool Is_function);

//Generates MULS
void Gen_multiply(bool Is_function);

//Generates concatenation
void Gen_concatenate(bool Is_function);

//Generates LTS operation
void Gen_less(bool Is_function);

//Generates GTS operation
void Gen_more(bool Is_function);

//Generates EQS operation
void Gen_equal(bool Is_function);

//Generates NOT EQS operation
void Gen_not_equal(bool Is_function);

//Generates less equal operation
void Gen_less_equal(bool Is_function);

//Generates more equal operation
void Gen_more_equal(bool Is_function);

//Generates input string
void Gen_inputs(bool Is_function);

//Generates input float
void Gen_inputf(bool Is_function);

//Generates input int
void Gen_inputi(bool Is_function);

//Generates store input to variable
void Gen_input_store(tKey id, bool Is_function);

//Generates push input
void Gen_push_input(tKey id, bool Is_function);

//Generates input pop
void Gen_input_pop(tKey id, bool Is_function);

//Generates false expression
void Gen_false(bool Is_function);

//Generates check return type
void Gen_check_type(tKey id, bool Is_function);

//Generates check parameters type
void Gen_check_param_type(tKey id);

//Generates check 2 parameters type
void Gen_check_2_param_type();

//Generate check for == & !=
void Gen_check_eq_neq_types(bool equal);

//Generates function return
void Gen_function_retval(tKey id, bool Is_function);

#endif

