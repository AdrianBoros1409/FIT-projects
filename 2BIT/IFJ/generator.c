/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @file generator.c
 * @brief Code Generator implementation
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */


#include "generator.h"

string code;    //String for generating code outside main
string main_code;   //String for generating code within main
bool Is_allocated = FALSE;
int Call_counter = 0;
int Var_counter = 0;
int Div_counter = 0;

// Free strings from memory
void Free_gen_strings(){
    if (code.str != NULL) {
        strFree(&code);
    }
    if (main_code.str != NULL) {
        strFree(&main_code);
    }
}

// Adding integer to generated code
void Add_int(int num, bool Is_function){
    char temp[SCHAR_MAX];
    sprintf(temp, "%d", num);
    if (Is_function)
        string_concat(&code, temp);
    else
        string_concat(&main_code, temp);
}

//Only print the generated code, when no error occurred
void Print_code(){
    Gen_main_end();
    printf("%s", code.str);
    printf("%s", main_code.str);

}

//Generates IFJ18 header, global variables and jump to main
void Start_code()
{
    strInit(&code);
    string_concat(&code, ".IFJcode18\n");
    string_concat(&code, "DEFVAR GF@%print\n");
    string_concat(&code, "DEFVAR GF@%op1\n");
    string_concat(&code, "DEFVAR GF@%op2\n");
    string_concat(&code, "DEFVAR GF@%op3\n");
    string_concat(&code, "DEFVAR GF@%expression\n");
    string_concat(&code, "DEFVAR GF@%input\n");
    string_concat(&code, "DEFVAR GF@%type\n");
    string_concat(&code, "DEFVAR GF@%return_type\n");
    string_concat(&code, "DEFVAR GF@%temp\n");
    string_concat(&code, "JUMP $$main\n");
    Gen_built_in_functions();
}

//Generates start of main Label only once
void Gen_main_start()
{
    if (Is_allocated == FALSE) {
        strInit(&main_code);
        Is_allocated = TRUE;
        string_concat(&main_code, "\nLABEL $$main\n");
        string_concat(&main_code, "CREATEFRAME\n");
        string_concat(&main_code, "PUSHFRAME\n");
    }
}

//Generates end of main Label
void Gen_main_end()
{
    string_concat(&main_code, "POPFRAME\n");
    string_concat(&main_code, "CLEARS\n");
}

//Generates start of function id
void Gen_function_start(tKey id)
{
    string_concat(&code, "\n#");
    string_concat(&code, id);
    string_concat(&code, "\nLABEL *");
    string_concat(&code, id);
    string_concat(&code, "*\n");
    string_concat(&code, "PUSHFRAME\n");
}

void Gen_frame(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "CREATEFRAME\n");
    else
        string_concat(&main_code, "CREATEFRAME\n");
}

//Generates end of function id
void Gen_function_end(tKey id)
{
    string_concat(&code, "#end function\n");
    string_concat(&code, "LABEL *");
    string_concat(&code, id);
    string_concat(&code, "_return*\n");
    string_concat(&code, "POPFRAME\n");
    string_concat(&code, "RETURN\n");
}

void Gen_call(tKey id, bool Is_function)
{
    if(Is_function == FALSE)
    {
        string_concat(&main_code, "CALL *");
        string_concat(&main_code, id);
        string_concat(&main_code, "*\n");
    }
    else
    {
        string_concat(&code, "CALL *");
        string_concat(&code, id);
        string_concat(&code, "*\n");
    }
}

void Gen_while_comment(int cnt, bool Is_function)
{
    char temp[SCHAR_MAX];
    if (Is_function) {
        string_concat(&code, "#While ");
        sprintf(temp, "%d", cnt);
        string_concat(&code, temp);
        string_concat(&code, "\n");
    }
    else {
        string_concat(&main_code, "#While ");
        sprintf(temp, "%d", cnt);
        string_concat(&main_code, temp);
        string_concat(&main_code, "\n");
    }
}

void Gen_while_label(tKey id, int num, int num2, bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "LABEL *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "*\n");
    }
    else
    {
        string_concat(&main_code, "LABEL *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "*\n");
    }
}

void Gen_while_start(tKey id, int num, int num2, bool Is_function)
{
    if(Is_function == TRUE)
    {
        string_concat(&code, "JUMPIFNEQ *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
    }
    else
    {
        string_concat(&main_code, "JUMPIFNEQ *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
    }
}

void Gen_while_end(tKey id, int num, int num2, bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "JUMP *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "*\n#Loop\n");
    }
    else
    {
        string_concat(&main_code, "JUMP *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "*\n#Loop\n");
    }
}

void Gen_if_start_comment(int cnt, bool Is_function)
{
    if (Is_function) {
        string_concat(&code, "#If statement ");
        Add_int(cnt, Is_function);
        string_concat(&code, "\n");
    }
    else {
        string_concat(&main_code, "#If statement ");
        Add_int(cnt, Is_function);
        string_concat(&main_code, "\n");
    }
}

void Gen_if_start(tKey id, int num, int num2, bool Is_function)
{
    if(Is_function == TRUE)
    {
        string_concat(&code, "JUMPIFNEQ *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "#then ");
        Add_int(num, Is_function);
        string_concat(&code, "\n");
    }
    else
    {
        string_concat(&main_code, "JUMPIFNEQ *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "#then ");
        Add_int(num, Is_function);
        string_concat(&main_code, "\n");
    }
}

void Gen_if_else(tKey id, int num, int num2, bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "#else ");
        Add_int(num, Is_function);
        string_concat(&code, "\nLABEL *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "*\n");
    }
    else
    {
        string_concat(&main_code, "#else ");
        Add_int(num, Is_function);
        string_concat(&main_code, "\nLABEL *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "*\n");
    }
}

void Gen_jump(tKey id, int num, int num2, bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "JUMP *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "*\n");
    }
    else
    {
        string_concat(&main_code, "JUMP *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "*\n");
    }
}

//Generates label id with two index numbers
void Gen_label(tKey id, int num , int num2, bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, id);
        string_concat(&code, "_");
        Add_int(num, Is_function);
        string_concat(&code, "_");
        Add_int(num2, Is_function);
    }
    else {
        string_concat(&main_code, id);
        string_concat(&main_code, "_");
        Add_int(num, Is_function);
        string_concat(&main_code, "_");
        Add_int(num2, Is_function);
    }
}

void Gen_if_label(tKey id, int num , int num2, bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "LABEL *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&code, "*\n");
    }
    else
    {
        string_concat(&main_code, "LABEL *");
        Gen_label(id, num, num2, Is_function);
        string_concat(&main_code, "*\n");
    }
}

//Generates variable declaration with implicit value nil
void Gen_var_declaration(tKey id, bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "DEFVAR LF@");
        string_concat(&code, id);
        string_concat(&code, "\n");
        string_concat(&code, "MOVE LF@");
        string_concat(&code, id);
        string_concat(&code, " nil@nil\n");
    }
    else {
        string_concat(&main_code, "DEFVAR LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, "\n");
        string_concat(&main_code, "MOVE LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " nil@nil\n");
    }
}

void Gen_pass_param(tKey id, int num, bool Is_function)
{
    if(Is_function) {
        string_concat(&code, "MOVE LF@");
        string_concat(&code, id);
        string_concat(&code, " LF@%");
        Add_int(num, Is_function);
        string_concat(&code, "\n");
    }
    else{
        string_concat(&main_code, "MOVE LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " LF@%");
        Add_int(num, Is_function);
        string_concat(&main_code, "\n");
    }
}

void Gen_function_print(bool Is_function)
{
    if (Is_function) {
        string_concat(&code, "WRITE GF@%print\n");
    } else
    {
        string_concat(&main_code, "WRITE GF@%print\n");
    }
}

void Gen_pushs(bool Is_function, Token tkn)
{
    if (Is_function)
    {
        string_concat(&code, "PUSHS ");
        Gen_type_value(tkn, Is_function);
        string_concat(&code, "\n");
    }
    else
    {
        string_concat(&main_code, "PUSHS ");
        Gen_type_value(tkn, Is_function);
        string_concat(&main_code, "\n");
    }
}

void Gen_print_pops(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "POPS GF@%print\n");
    }
    else
        string_concat(&main_code, "POPS GF@%print\n");
}

//Generates temporary stack before calling function and checks if parameter is not nil
void Gen_temp_stack(Token tkn, int num, bool Is_function)
{
    Var_counter++;
    if(Is_function){
        string_concat(&code, "DEFVAR TF@%");
        Add_int(num, Is_function);
        string_concat(&code, "\nMOVE TF@%");
        Add_int(num, Is_function);
        string_concat(&code, " ");
        Gen_type_value(tkn, Is_function);
        string_concat(&code, "\n");
        string_concat(&code, "TYPE GF@%temp TF@%");
        Add_int(num, Is_function);
        string_concat(&code, "\nEQ GF@%expression GF@%temp string@nil\n");
        string_concat(&code, "JUMPIFNEQ *param_not_nil_");
        Add_int(Var_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "EXIT int@4\n");
        string_concat(&code, "LABEL *param_not_nil_");
        Add_int(Var_counter, Is_function);
        string_concat(&code, "*\n");
    }
    else {
        string_concat(&main_code, "DEFVAR TF@%");
        Add_int(num, Is_function);
        string_concat(&main_code, "\nMOVE TF@%");
        Add_int(num, Is_function);
        string_concat(&main_code, " ");
        Gen_type_value(tkn, Is_function);
        string_concat(&main_code, "\n");
        string_concat(&main_code, "TYPE GF@%temp TF@%");
        Add_int(num, Is_function);
        string_concat(&main_code, "\nEQ GF@%expression GF@%temp string@nil\n");
        string_concat(&main_code, "JUMPIFNEQ *param_not_nil_");
        Add_int(Var_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "EXIT int@4\n");
        string_concat(&main_code, "LABEL *param_not_nil_");
        Add_int(Var_counter, Is_function);
        string_concat(&main_code, "*\n");
    }
}

void Gen_function_defvar()
{
    string_concat(&code, "DEFVAR LF@$return_val\n");
    string_concat(&code, "MOVE LF@$return_val nil@nil\n");
}

void Gen_store_value()
{
    string_concat(&code, "POPS GF@%expression\n");
}

void Gen_store_id(tKey id)
{
    string_concat(&code, "PUSHS LF@");
    string_concat(&code, id);
    string_concat(&code, "\nPOPS GF@%expression\n");
}

void Gen_function_return()
{
    string_concat(&code, "MOVE LF@$return_val GF@%expression\n");
}

void Gen_function_retval(tKey id, bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "MOVE LF@");
        string_concat(&code, id);
        string_concat(&code, " TF@$return_val\n");
    }
    else
    {
        string_concat(&main_code, "MOVE LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " TF@$return_val\n");
    }
}

//Generates actual term value
void Gen_type_value(Token tkn, bool Is_function)
{
    char temp[SCHAR_MAX];
    switch (tkn.type) {
        case NUM:
            if (Is_function) {
                string_concat(&code, "int@");
                sprintf(temp, "%d", tkn.atr.number);
                string_concat(&code, temp);
            } else {
                string_concat(&main_code, "int@");
                sprintf(temp, "%d", tkn.atr.number);
                string_concat(&main_code, temp);
            }
            break;
        case NUM_FLOAT:
            if (Is_function) {
                string_concat(&code, "float@");
                sprintf(temp, "%a", tkn.atr.numberD);
                string_concat(&code, temp);
            } else {
                string_concat(&main_code, "float@");
                sprintf(temp, "%a", tkn.atr.numberD);
                string_concat(&main_code, temp);
            }
            break;
        case STRING:
            if(Is_function)
            {
                string_concat(&code, "string@");
                Format_string(tkn.atr.str, Is_function);
            }
            else
            {
                string_concat(&main_code, "string@");
                Format_string(tkn.atr.str, Is_function);
            }
            break;
        case ID:
            if(Is_function)
            {
                string_concat(&code, "LF@");
                string_concat(&code, tkn.atr.str->str);
            }
            else
            {
                string_concat(&main_code, "LF@");
                string_concat(&main_code, tkn.atr.str->str);
            }
        default:
            break;
    }
}

void Gen_expression_pop(tKey id, bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "POPS LF@");
        string_concat(&code, id);
        string_concat(&code, "\n");

    } else {
        string_concat(&main_code, "POPS LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, "\n");
    }
}

//Format special symbols of string format_str
void Format_string(string* format_str, bool Is_function)
{
    int max=strGetLength(format_str);
    char temp[max];
     for (int i = 0; i < max; i++) {
         if(format_str->str[i] <= 32 || format_str->str[i] == '#' || format_str->str[i] == '\\') {
             if (Is_function)
                 strAddChar(&code, '\\');
             else
                 strAddChar(&main_code, '\\');
             sprintf(temp, "%03d", format_str->str[i]);
             if (Is_function)
                 string_concat(&code, temp);
             else
                 string_concat(&main_code, temp);
         }
         else {
             if (Is_function)
                 strAddChar(&code, format_str->str[i]);
             else
                 strAddChar(&main_code, format_str->str[i]);
         }
     }
 }

void Gen_int2floats(bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "INT2FLOATS\n");
    }
    else
    {
        string_concat(&main_code, "INT2FLOATS\n");
    }
}

void Gen_float2ints(bool Is_function)
{
     if(Is_function)
     {
         string_concat(&code, "FLOAT2INTS\n");
     }
     else
     {
         string_concat(&main_code, "FLOAT2INTS\n");
     }
}

void Gen_float2ints_op2(bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "POPS GF@%op1\n");
        Gen_float2ints(Is_function);
        string_concat(&code, "PUSHS GF@%op1\n");
    }
    else
    {
        string_concat(&main_code, "POPS GF@%op1\n");
        Gen_float2ints(Is_function);
        string_concat(&main_code, "PUSHS GF@%op1\n");
    }
}

void Gen_int2floats_op2(bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "POPS GF@%op1\n");
        Gen_int2floats(Is_function);
        string_concat(&code, "PUSHS GF@%op1\n");
    }
    else
    {
        string_concat(&main_code, "POPS GF@%op1\n");
        Gen_int2floats(Is_function);
        string_concat(&main_code, "PUSHS GF@%op1\n");
    }
}

void Gen_int2float(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "INT2FLOAT TF@$return_val TF@$return_val\n");
    else
        string_concat(&main_code, "INT2FLOAT TF@$return_val TF@$return_val\n");
}

void Gen_minus(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "SUBS\n");
    else
        string_concat(&main_code, "SUBS\n");
}

void Gen_plus(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "ADDS\n");
    else
        string_concat(&main_code, "ADDS\n");
}

//Generates function to control dividing by zero
void Gen_divide_check(bool Is_function)
{
    Div_counter ++;
    if (Is_function)
    {
        string_concat(&code, "POPS GF@%temp\n");
        string_concat(&code, "TYPE GF@%type GF@%temp\n");
        string_concat(&code, "EQ GF@%expression GF@%type string@int\n");
        string_concat(&code, "JUMPIFNEQ *check_float_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "LABEL *check_int_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EQ GF@%expression GF@%temp int@0\n");
        string_concat(&code, "JUMPIFNEQ *check_ok_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "JUMP *check_error_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *check_float_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EQ GF@%expression GF@%temp float@0x0.000000p+0\n");
        string_concat(&code, "JUMPIFNEQ *check_ok_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "LABEL *check_error_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EXIT int@9\n");
        string_concat(&code, "LABEL *check_ok_");
        Add_int(Div_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "PUSHS GF@%temp\n");
    }
    else
    {
        string_concat(&main_code, "POPS GF@%temp\n");
        string_concat(&main_code, "TYPE GF@%type GF@%temp\n");
        string_concat(&main_code, "EQ GF@%expression GF@%type string@int\n");
        string_concat(&main_code, "JUMPIFNEQ *check_float_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "LABEL *check_int_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EQ GF@%expression GF@%temp int@0\n");
        string_concat(&main_code, "JUMPIFNEQ *check_ok_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "JUMP *check_error_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *check_float_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EQ GF@%expression GF@%temp float@0x0.000000p+0\n");
        string_concat(&main_code, "JUMPIFNEQ *check_ok_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "LABEL *check_error_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EXIT int@9\n");
        string_concat(&main_code, "LABEL *check_ok_");
        Add_int(Div_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "PUSHS GF@%temp\n");
    }
}

void Gen_divide(bool Is_function)
{
    Gen_divide_check(Is_function);
    if (Is_function)
        string_concat(&code, "DIVS\n");
    else
        string_concat(&main_code, "DIVS\n");
}

void Gen_divide_int(bool Is_function)
{
    Gen_divide_check(Is_function);
    if (Is_function)
        string_concat(&code, "IDIVS\n");
    else
        string_concat(&main_code, "IDIVS\n");
}

void Gen_multiply(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "MULS\n");
    else
        string_concat(&main_code, "MULS\n");
}

void Gen_concatenate(bool Is_function)
{
    if(Is_function)
    {
        string_concat(&code, "POPS GF@%op2\n");
        string_concat(&code, "POPS GF@%op3\n");
        string_concat(&code, "CONCAT GF@%op1 GF@%op3 GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op1\n");
    }
    else
    {
        string_concat(&main_code, "POPS GF@%op2\n");
        string_concat(&main_code, "POPS GF@%op3\n");
        string_concat(&main_code, "CONCAT GF@%op1 GF@%op3 GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op1\n");
    }
}

void Gen_less(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "LTS\n");
        string_concat(&code, "POPS GF@%expression\n");
    }
    else
    {
        string_concat(&main_code, "LTS\n");
        string_concat(&main_code, "POPS GF@%expression\n");
    }
}

void Gen_more(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "GTS\n");
        string_concat(&code, "POPS GF@%expression\n");
    }
    else
    {
        string_concat(&main_code, "GTS\n");
        string_concat(&main_code, "POPS GF@%expression\n");
    }
}

void Gen_equal(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "EQS\n");
        string_concat(&code, "POPS GF@%expression\n");
    }
    else
    {
        string_concat(&main_code, "EQS\n");
        string_concat(&main_code, "POPS GF@%expression\n");
    }
}

void Gen_not_equal(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "EQS\n");
        string_concat(&code, "NOTS\n");
        string_concat(&code, "POPS GF@%expression\n");
    }
    else
    {
        string_concat(&main_code, "EQS\n");
        string_concat(&main_code, "NOTS\n");
        string_concat(&main_code, "POPS GF@%expression\n");
    }
}

void Gen_less_equal(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "POPS GF@%op1\n");
        string_concat(&code, "POPS GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op1\n");
        string_concat(&code, "EQS\n");
        string_concat(&code, "PUSHS GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op1\n");
        string_concat(&code, "LTS\n");
        string_concat(&code, "ORS\n");
        string_concat(&code, "POPS GF@%expression\n");
    }
    else
    {
        string_concat(&main_code, "POPS GF@%op1\n");
        string_concat(&main_code, "POPS GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op1\n");
        string_concat(&main_code, "EQS\n");
        string_concat(&main_code, "PUSHS GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op1\n");
        string_concat(&main_code, "LTS\n");
        string_concat(&main_code, "ORS\n");
        string_concat(&main_code, "POPS GF@%expression\n");
    }
}

void Gen_more_equal(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "POPS GF@%op1\n");
        string_concat(&code, "POPS GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op1\n");
        string_concat(&code, "EQS\n");
        string_concat(&code, "PUSHS GF@%op2\n");
        string_concat(&code, "PUSHS GF@%op1\n");
        string_concat(&code, "GTS\n");
        string_concat(&code, "ORS\n");
        string_concat(&code, "POPS GF@%expression\n");
    }
    else
    {
        string_concat(&main_code, "POPS GF@%op1\n");
        string_concat(&main_code, "POPS GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op1\n");
        string_concat(&main_code, "EQS\n");
        string_concat(&main_code, "PUSHS GF@%op2\n");
        string_concat(&main_code, "PUSHS GF@%op1\n");
        string_concat(&main_code, "GTS\n");
        string_concat(&main_code, "ORS\n");
        string_concat(&main_code, "POPS GF@%expression\n");
    }
}

void Gen_inputs(bool Is_function)
{
    if (Is_function)
    {
        string_concat(&code, "READ GF@%input string\n");
    } else
        string_concat(&main_code, "READ GF@%input string\n");
}

void Gen_inputf(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "READ GF@%input float\n");
    else
        string_concat(&main_code, "READ GF@%input float\n");
}

void Gen_inputi(bool Is_function)
{
    if (Is_function)
        string_concat(&code, "READ GF@%input int\n");
    else
        string_concat(&main_code, "READ GF@%input int\n");
}

void Gen_input_store(tKey id, bool Is_function)
{
    if (Is_function) {
        string_concat(&code, "MOVE LF@");
        string_concat(&code, id);
        string_concat(&code, " GF@%input\n");
    } else {
        string_concat(&main_code, "MOVE LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " GF@%input\n");
    }
}

void Gen_push_input(tKey id, bool Is_function)
{
    if (Is_function){
        string_concat(&code, "PUSHS LF@");
        string_concat(&code, id);
        string_concat(&code, "\n");
    }
    else{
        string_concat(&main_code, "PUSHS LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, "\n");
    }
}

void Gen_input_pop(tKey id, bool Is_function){
    if(Is_function){
        string_concat(&code, "POPS LF@");
        string_concat(&code, id);
        string_concat(&code, "\n");
    }
    else{
        string_concat(&main_code, "POPS LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, "\n");
    }
}

void Gen_false(bool Is_function)
{
    if(Is_function){
        string_concat(&code, "MOVE GF@%expression bool@false\n");

    }
    else{
        string_concat(&main_code, "MOVE GF@%expression bool@false\n");
    }
}

/* Generates function to control if returning type of function
can be stored into value*/
void Gen_check_type(tKey id, bool Is_function)
{
    Call_counter++;
    if (Is_function)
    {
        string_concat(&code, "#Check return_val type\n");
        string_concat(&code, "TYPE GF@%type LF@");
        string_concat(&code, id);
        string_concat(&code, "\n");
        string_concat(&code, "TYPE GF@%return_type TF@$return_val\n");
        string_concat(&code, "EQ GF@%expression GF@%return_type GF@%type\n");
        string_concat(&code, "JUMPIFEQ *equal_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "EQ GF@%expression GF@%type string@nil\n");
        string_concat(&code, "JUMPIFEQ *equal_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "LABEL *check_int_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EQ GF@%expression GF@%type string@int\n");
        string_concat(&code, "JUMPIFNEQ *check_float_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "EQ GF@%expression GF@%return_type string@float\n");
        string_concat(&code, "JUMPIFEQ *int2float_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "EQ GF@%expression GF@%return_type string@nil\n");
        string_concat(&code, "JUMPIFEQ *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "JUMP *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *int2float_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "FLOAT2INT LF@");
        string_concat(&code, id);
        string_concat(&code, " TF@$return_val\nJUMP *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *check_float_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EQ GF@%expression GF@%type string@float\n");
        string_concat(&code, "JUMPIFNEQ *check_string_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "EQ GF@%expression GF@%return_type string@int\n");
        string_concat(&code, "JUMPIFEQ *float2int_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "EQ GF@%expression GF@%return_type string@nil\n");
        string_concat(&code, "JUMPIFEQ *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "JUMP *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *float2int_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "INT2FLOAT LF@");
        string_concat(&code, id);
        string_concat(&code, " TF@$return_val\nJUMP *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *check_string_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EQ GF@%expression GF@%return_type string@nil\n");
        string_concat(&code, "JUMPIFEQ *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "* GF@%expression bool@true\n");
        string_concat(&code, "JUMP *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *equal_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "MOVE LF@");
        string_concat(&code, id);
        string_concat(&code, " TF@$return_val\n");
        string_concat(&code, "JUMP *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "LABEL *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n");
        string_concat(&code, "EXIT int@3\n");
        string_concat(&code, "LABEL *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&code, "*\n\n");
    }
    else {
        string_concat(&main_code, "#Check return_val type\n");
        string_concat(&main_code, "TYPE GF@%type LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, "\n");
        string_concat(&main_code, "TYPE GF@%return_type TF@$return_val\n");
        string_concat(&main_code, "EQ GF@%expression GF@%return_type GF@%type\n");
        string_concat(&main_code, "JUMPIFEQ *equal_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "EQ GF@%expression GF@%type string@nil\n");
        string_concat(&main_code, "JUMPIFEQ *equal_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "LABEL *check_int_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EQ GF@%expression GF@%type string@int\n");
        string_concat(&main_code, "JUMPIFNEQ *check_float_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "EQ GF@%expression GF@%return_type string@float\n");
        string_concat(&main_code, "JUMPIFEQ *int2float_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "EQ GF@%expression GF@%return_type string@nil\n");
        string_concat(&main_code, "JUMPIFEQ *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "JUMP *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *int2float_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "FLOAT2INT LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " TF@$return_val\nJUMP *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *check_float_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EQ GF@%expression GF@%type string@float\n");
        string_concat(&main_code, "JUMPIFNEQ *check_string_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "EQ GF@%expression GF@%return_type string@int\n");
        string_concat(&main_code, "JUMPIFEQ *float2int_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "EQ GF@%expression GF@%return_type string@nil\n");
        string_concat(&main_code, "JUMPIFEQ *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "JUMP *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *float2int_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "INT2FLOAT LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " TF@$return_val\nJUMP *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *check_string_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EQ GF@%expression GF@%return_type string@nil\n");
        string_concat(&main_code, "JUMPIFEQ *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "* GF@%expression bool@true\n");
        string_concat(&main_code, "JUMP *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *equal_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "MOVE LF@");
        string_concat(&main_code, id);
        string_concat(&main_code, " TF@$return_val\n");
        string_concat(&main_code, "JUMP *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "LABEL *error_types_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n");
        string_concat(&main_code, "EXIT int@3\n");
        string_concat(&main_code, "LABEL *end_");
        Add_int(Call_counter, Is_function);
        string_concat(&main_code, "*\n\n");
    }
}

//Generates function to control parameter  types
void Gen_check_param_type(tKey id)
{
    Var_counter ++;
    string_concat(&code, "\n#Check parameter type\n");
    string_concat(&code, "POPS GF@%temp\n");
    string_concat(&code, "TYPE GF@%type LF@");
    string_concat(&code, id);
    string_concat(&code, "\nTYPE GF@%return_type GF@%temp\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@nil\n");
    string_concat(&code, "JUMPIFEQ *assign_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type GF@%type\n");
    string_concat(&code, "JUMPIFEQ *assign_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@string\n");
    string_concat(&code, "JUMPIFEQ *assign_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@int\n");
    string_concat(&code, "JUMPIFNEQ *assign_float_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@float\n");
    string_concat(&code, "JUMPIFNEQ *assign_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "FLOAT2INT GF@%temp GF@%temp\n");
    string_concat(&code, "JUMP *assign_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "LABEL *assign_float_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@int\n");
    string_concat(&code, "JUMPIFNEQ *assign_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "INT2FLOAT GF@%temp GF@%temp\n");
    string_concat(&code, "JUMP *assign_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "LABEL *assign_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "EXIT int@4\n");
    string_concat(&code, "LABEL *assign_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "PUSHS GF@%temp\n\n");
}


//Generates function to check types of two terms in operations except == !=
void Gen_check_2_param_type()
{
    Var_counter ++;
    string_concat(&code, "\n#Check 2 parameter type\n");
    string_concat(&code, "POPS GF@%op1\n");
    string_concat(&code, "TYPE GF@%type GF@%op1\n");
    string_concat(&code, "POPS GF@%op2\n");
    string_concat(&code, "TYPE GF@%return_type GF@%op2\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@nil\n");
    string_concat(&code, "JUMPIFEQ *params_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@nil\n");
    string_concat(&code, "JUMPIFEQ *params_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type GF@%return_type\n");
    string_concat(&code, "JUMPIFEQ *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@string\n");
    string_concat(&code, "JUMPIFEQ *params_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@int\n");
    string_concat(&code, "JUMPIFEQ *param1_int_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@string\n");
    string_concat(&code, "JUMPIFEQ *params_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "INT2FLOAT GF@%op2 GF@%op2\n");
    string_concat(&code, "JUMP *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "LABEL *param1_int_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@string\n");
    string_concat(&code, "JUMPIFEQ *params_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "INT2FLOAT GF@%op1 GF@%op1\n");
    string_concat(&code, "JUMP *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "LABEL *params_error_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "EXIT int@4\n");
    string_concat(&code, "LABEL *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "PUSHS GF@%op2\n");
    string_concat(&code, "PUSHS GF@%op1\n\n");
}

//Generates function to check types of two terms within == or != comparision
void Gen_check_eq_neq_types(bool equal)
{
    Var_counter ++;
    string_concat(&code, "\n#Check 2 parameter type for == / !=\n");
    string_concat(&code, "POPS GF@%op1\n");
    string_concat(&code, "TYPE GF@%type GF@%op1\n");
    string_concat(&code, "POPS GF@%op2\n");
    string_concat(&code, "TYPE GF@%return_type GF@%op2\n");
    string_concat(&code, "EQ GF@%expression GF@%type GF@%return_type\n");
    string_concat(&code, "JUMPIFEQ *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@nil\n");
    string_concat(&code, "JUMPIFEQ *params_false_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@nil\n");
    string_concat(&code, "JUMPIFEQ *params_false_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@string\n");
    string_concat(&code, "JUMPIFEQ *params_false_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%type string@int\n");
    string_concat(&code, "JUMPIFEQ *param1_int_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@string\n");
    string_concat(&code, "JUMPIFEQ *params_false_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "INT2FLOAT GF@%op2 GF@%op2\n");
    string_concat(&code, "JUMP *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "LABEL *param1_int_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "EQ GF@%expression GF@%return_type string@string\n");
    string_concat(&code, "JUMPIFEQ *params_false_");
    Add_int(Var_counter, true);
    string_concat(&code, "* GF@%expression bool@true\n");
    string_concat(&code, "INT2FLOAT GF@%op1 GF@%op1\n");
    string_concat(&code, "JUMP *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "LABEL *params_false_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    Gen_false(true);
    string_concat(&code, "JUMP *params_end_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\nLABEL *param1_param2_ok_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n");
    string_concat(&code, "PUSHS GF@%op2\n");
    string_concat(&code, "PUSHS GF@%op1\n\n");
    if (equal)
        string_concat(&code, "EQS\n");
    else
        string_concat(&code, "EQS\nNOT\n");
    string_concat(&code, "LABEL *params_end_");
    Add_int(Var_counter, true);
    string_concat(&code, "*\n\n");
}

//Generates built-in functions
void Gen_built_in_functions()
{
    string_concat(&code, "\n#length(s)\n");
    string_concat(&code, "LABEL *length*\n");
    string_concat(&code, "PUSHFRAME\n");
    string_concat(&code, "DEFVAR LF@$return_val\n");
    string_concat(&code, "STRLEN LF@$return_val LF@%0\n");
    string_concat(&code, "POPFRAME\n");
    string_concat(&code, "RETURN\n");

    string_concat(&code, "\n#substr(s, i, n)\n");
    string_concat(&code, "LABEL *substr*\n");
    string_concat(&code, "PUSHFRAME\n");
    string_concat(&code, "DEFVAR LF@$return_val\n");
    string_concat(&code, "DEFVAR LF@length\n");
    string_concat(&code, "STRLEN LF@length LF@%0\n");
    string_concat(&code, "DEFVAR LF@exit\n");
    string_concat(&code, "GT LF@exit LF@%1 LF@length\n");
    string_concat(&code, "JUMPIFEQ *substr_nil* LF@exit bool@true\n");
    string_concat(&code, "LT LF@exit LF@%1 int@0\n");
    string_concat(&code, "JUMPIFEQ *substr_nil* LF@exit bool@true\n");
    string_concat(&code, "LT LF@exit LF@%2 int@0\n");
    string_concat(&code, "JUMPIFEQ *substr_nil* LF@exit bool@true\n");
    string_concat(&code, "DEFVAR LF@length_max\n");
    string_concat(&code, "MOVE LF@length_max LF@length\n");
    string_concat(&code, "SUB LF@length_max LF@length_max LF@%1\n");
    string_concat(&code, "DEFVAR LF@length_max_cond\n");
    string_concat(&code, "GT LF@length_max_cond LF@%2 LF@length_max\n");
    string_concat(&code, "JUMPIFEQ *substr_n* LF@length_max_cond bool@true\n");
    string_concat(&code, "JUMP *substr_prep*\n");
    string_concat(&code, "LABEL *substr_n*\n");
    string_concat(&code, "MOVE LF@%2 LF@length_max\n");
    string_concat(&code, "LABEL *substr_prep*\n");
    string_concat(&code, "MOVE LF@$return_val string@\n");
    string_concat(&code, "DEFVAR LF@char\n");
    string_concat(&code, "DEFVAR LF@index\n");
    string_concat(&code, "MOVE LF@index LF@%1\n");
    string_concat(&code, "DEFVAR LF@cycle_cond\n");
    string_concat(&code, "LABEL *substr_cycle*\n");
    string_concat(&code, "GETCHAR LF@char LF@%0 LF@index\n");
    string_concat(&code, "CONCAT LF@$return_val LF@$return_val LF@char\n");
    string_concat(&code, "ADD LF@index LF@index int@1\n");
    string_concat(&code, "SUB LF@%2 LF@%2 int@1\n");
    string_concat(&code, "GT LF@cycle_cond LF@%2 int@0\n");
    string_concat(&code, "JUMPIFEQ *substr_cycle* LF@cycle_cond bool@true\n");
    string_concat(&code, "JUMP *substr_return*\n");
    string_concat(&code, "LABEL *substr_nil*\n");
    string_concat(&code, "MOVE LF@$return_val nil@nil\n");
    string_concat(&code, "LABEL *substr_return*\n");
    string_concat(&code, "POPFRAME\n");
    string_concat(&code, "RETURN\n");

    string_concat(&code, "\n#ord(s, i)\n");
    string_concat(&code, "LABEL *ord*\n");
    string_concat(&code, "PUSHFRAME\n");
    string_concat(&code, "DEFVAR LF@$return_val\n");
    string_concat(&code, "MOVE LF@$return_val nil@nil\n");
    string_concat(&code, "DEFVAR LF@exit\n");
    string_concat(&code, "LT LF@exit LF@%1 int@0\n");
    string_concat(&code, "JUMPIFEQ *ord_return* LF@exit bool@true\n");
    string_concat(&code, "DEFVAR LF@length\n");
    string_concat(&code, "STRLEN LF@length LF@%0\n");
    string_concat(&code, "SUB LF@length  LF@length int@1\n");
    string_concat(&code, "GT LF@exit LF@%1 LF@length\n");
    string_concat(&code, "JUMPIFEQ *ord_return* LF@exit bool@true\n");
    string_concat(&code, "STRI2INT LF@$return_val LF@%0 LF@%1\n");
    string_concat(&code, "LABEL *ord_return*\n");
    string_concat(&code, "POPFRAME\n");
    string_concat(&code, "RETURN\n");

    string_concat(&code, "\n#chr(i)\n");
    string_concat(&code, "LABEL *chr*\n");
    string_concat(&code, "PUSHFRAME\n");
    string_concat(&code, "DEFVAR LF@$return_val\n");
    string_concat(&code, "MOVE LF@$return_val string@\n");
    string_concat(&code, "DEFVAR LF@exit\n");
    string_concat(&code, "LT LF@exit LF@%0 int@0\n");
    string_concat(&code, "JUMPIFEQ *chr_error* LF@exit bool@true\n");
    string_concat(&code, "GT LF@exit LF@%0 int@255\n");
    string_concat(&code, "JUMPIFEQ *chr_exit* LF@exit bool@true\n");
    string_concat(&code, "INT2CHAR LF@$return_val LF@%0\n");
    string_concat(&code, "JUMP *chr_return\n");
    string_concat(&code, "LABEL *chr_error*\n");
    string_concat(&code, "EXIT int@4\n");
    string_concat(&code, "LABEL *chr_return*\n");
    string_concat(&code, "POPFRAME\n");
    string_concat(&code, "RETURN\n");

}
