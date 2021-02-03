/**
 * Projekt: Implementácia prekladača imperatívneho jazyka IFJ18
 * @brief Scanner implementation
 * @file scanner.c
 * @brief Tím 112 / varianta II
 * @author Adrián Boros <xboros03@stud.fit.vutbr.cz>
 * @author Adam Abrahám <xabrah04@stud.fit.vutbr.cz>
 * @author Matouš Sloboda <xslobo04@stud.fit.vutbr.cz>
 * @author Tomáš Žigo <xzigot00@stud.fit.vutbr.cz>
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"

FILE *source;
string str;
Token tkn;

void setSource(FILE *f) {
    source = f;
}

// Line begin
bool LineBegin = true;
// Decimal number
bool isDouble = false;
Token scan(){
    char *keyWords[KEYWORD_SUM] = {"def", "do", "else", "end", "if", "not", "nil", "then", "while", "print",
                                   "inputs", "inputi", "inputf", "length", "substr", "ord", "chr"};
    // Start of block commentary
    char *comments_start = "=begin";
    // End of block commentary
    char *comments_end = "=end";

    // Dynamic string init
    if(strInit(&str) == 1){
        tkn.type = 99;
        return tkn;
    }

    // Beginning state
    int stateFlag = START;
    int c;
    int tmp = 0;
    while ((c = getc(source)) != EOF) {
        // Final state automata
        switch(stateFlag) {
            case START:
                // End of line
                if(c == '\n'){
                    LineBegin = true;
                    tkn.type = EOL;
                    return tkn;
                }
                // Ignore white spaces
                else if (isspace(c)) {
                    stateFlag = START;
                    LineBegin = false;
                }
                // Start of line commment
                else if(c == '#'){
                    stateFlag = LINE_COMMENT;
                }
                // Identifier of keyword
                else if(isalpha(c) || c == '_'){
                    if (c >= 'A' && c <= 'Z') {
                        tkn.type = LEXICAL_ERROR;
                        return tkn;
                    } else {
                        strAddChar(&str, (char)c);
                        stateFlag = ID;
                    }
                }
                // String
                else if(c == '"'){
                    stateFlag = STRING_START;
                }
                // Number
                else if(isdigit(c)){
                    strAddChar(&str, (char)c);
                    // Reduntant 0 in the beginning = error
                    if(c == '0'){
                        tmp = getc(source);
                        if(isdigit(tmp)){
                            tkn.type = LEXICAL_ERROR;
                            return tkn;
                        } else{
                            ungetc(tmp,source);
                            stateFlag = NUM;
                        }
                    }
                    stateFlag = NUM;
                }
                // Assignemnt, equal or start of block commentary
                else if(c == '='){
                    strAddChar(&str, (char)c);
                    stateFlag = ASSIGNMENT;
                }
                // Operator +
                else if(c == '+'){
                    tkn.type = PLUS;
                    return tkn;
                }
                // Operator -
                else if(c == '-'){
                    tkn.type = MINUS;
                    return tkn;
                }
                // Operator *
                else if(c == '*'){
                    tkn.type = MULTIPLY;
                    return tkn;
                }
                // Operator /
                else if(c == '/'){
                    tkn.type = DIVIDE;
                    return tkn;
                }
                // Left bracket
                else if(c == '('){
                    tkn.type = ROUND_BRACKET_LEFT;
                    return tkn;
                }
                // Right bracket
                else if(c == ')'){
                    tkn.type = ROUND_BRACKET_RIGHT;
                    return tkn;
                }
                // Comma
                else if(c == ','){
                    tkn.type = COMMA;
                    return tkn;
                }
                // Operator >
                else if(c == '>'){
                    strAddChar(&str, (char)c);
                    stateFlag = MORE_THAN;
                }
                // Operator >
                else if(c == '<'){
                    strAddChar(&str, (char)c);
                    stateFlag = LESS_THAN;
                }
                // End of file
                else if(c == EOF){
                    tkn.type = END_OF_FILE;
                    return tkn;
                }
                // Operator !
                else if(c == '!'){
                    strAddChar(&str, (char)c);
                    stateFlag = NEGATE;
                }
                else{
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                break;

            case LINE_COMMENT:
                // Skip characters to the end of the line
                while (c != '\n') {
                    c = getc(source);
                    if(c == EOF) {
                        tkn.type = LEXICAL_ERROR;
                        return tkn;
                    }
                }
                tkn.type = EOL;
                return tkn;

            case BLOCK_COMMENT:
                if(isalpha(c))
                    strAddChar(&str,(char)c);
                else{
                    ungetc(c, source);
                    // Compare word with keyword "=begin"
                    if(strCmpConstStr(&str,comments_start) == 0) {
                        // If equal we are in block comment
                        stateFlag = COMMENT_IGNORE;
                        strClear(&str);
                    }
                    else
                    {
                        tkn.type = LEXICAL_ERROR;
                        return tkn;
                    }
                }
                break;

            case COMMENT_IGNORE:
                // Skip characters till we don´t get '='
                while (c != '='){
                    tmp = c;
                    c = getc(source);
                    if(c == EOF){
                        tkn.type = LEXICAL_ERROR;
                        return tkn;
                    }
                }
                // If in c is '=' and character before is is EOL
                if(tmp == '\n')
                    strAddChar(&str, (char)c);  // Add char '='
                while(isalpha(c) || c == '='){
                    c = getc(source);
                    if(isalpha(c))
                        strAddChar(&str,(char)c);  // Store chars
                    else{
                        ungetc(c,source);
                    }
                }
                // Compare content of str with keyword "=end", if equal skip character to end of line
                if(strCmpConstStr(&str,comments_end) == 0) {
                    while (c != '\n')
                        c = getc(source);
                    stateFlag = START;
                    strClear(&str);
                }
                // If we don´t find '=', continue in skipping chars
                else{
                    stateFlag = COMMENT_IGNORE;
                    strClear(&str);
                }
                break;
            case ID:
                // Identifier can contain just numbers, letters and char '_' or '?!' in the end
                if(isalnum(c) || c == '_') {
                    strAddChar(&str, (char) c);
                }
                // Check if ? or ! are in the end of function identifier, else error
                else if(c == '?' || c == '!'){
                    tmp = getc(source);
                    if(tmp == '(' || isspace(tmp) || tmp == '\n') {
                        strAddChar(&str, (char) c);
                        ungetc(tmp,source);
                    }
                    else{
                        tkn.type = LEXICAL_ERROR;
                        return tkn;
                    }
                }
                else if(isspace(c) || c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' ||
                        c == '=' || c == ',' || c == '(' || c == ')'){
                    ungetc(c, source);
                    // Check if is not a keyword
                    for (int i = 0; i < KEYWORD_SUM; i++) {
                        if (strCmpConstStr(&str, keyWords[i]) == 0){
                            tkn.type = KEY_WORD;
                            tkn.atr.str = &str;
                            return tkn;
                        }
                        else{
                            string temp;
                            strInit(&temp);
                            for (int j = 0; j < str.length; ++j) {
                                strAddChar(&temp, (char)tolower(str.str[j]));
                            }
                            if (strCmpConstStr(&temp, keyWords[i]) == 0) {
                                tkn.type = LEXICAL_ERROR;
                                return tkn;
                            }
                            strFree(&temp);
                        }
                    }
                    // Return identifier
                    tkn.type = ID;
                    tkn.atr.str = &str;
                    LineBegin = false;
                    return tkn;
                }
                else{
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                break;
            case STRING_START:
                if(c <= 31){
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                // Empty string ""
                else if(c == '"'){
                    tkn.type = STRING;
                    tkn.atr.str = &str;
                    return tkn;
                }
                // If string contain escape sequence
                else if(c == '\\')
                    stateFlag = ESCAPE_SEQUENCE;
                else if(c == '\n' || c == EOF){
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                else
                    // String contains characters
                    strAddChar(&str, (char)c);
                break;
            case ESCAPE_SEQUENCE:
                switch(c){
                    case 'n':
                        c = '\n';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    case '"':
                        c = '\"';
                        break;
                    case '\\':
                        c = '\\';
                        break;
                    case 's':
                        c = ' ';
                        break;
                    case 'x':
                        c = getc(source);
                        // Chars can be in range 0-9, A-F or a-f
                        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                            char hexaArray[2];
                            hexaArray[0] = (char)c;
                            c = getc(source);
                            // Check second char
                            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                                hexaArray[1] = (char)c;
                            }
                            else {
                                ungetc(c, source);
                            }
                            long long hexa = strtol(hexaArray, NULL, 16);
                            c = (char)hexa;
                        } else {
                            // If not a char, error
                            tkn.type = LEXICAL_ERROR;
                            return tkn;
                        }
                        break;
                    default:
                        break;
                }
                strAddChar(&str, (char)c);
                stateFlag = STRING_START;
                break;
            case NUM:
                if(isdigit(c))
                    strAddChar(&str, (char) c);
                // Decimal number
                else if (c == '.') {
                    strAddChar(&str, (char)c);
                    isDouble = true;
                    stateFlag = NUM_POINT_FLOAT;
                }
                // Number in exponential form
                else if(tolower(c) == 'e') {
                    strAddChar(&str, (char)c);
                    stateFlag = NUM_EXPONENT;
                }
                else{
                    ungetc(c, source);
                    tkn.type = NUM;
                    tkn.atr.number = (int)strtol(str.str, NULL, 10);
                    LineBegin = false;
                    return tkn;
                }
                break;
            case NUM_POINT_FLOAT:
                if(isdigit(c)){
                    strAddChar(&str, (char)c);
                    stateFlag = NUM_FLOAT;
                }
                else{
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                break;
            case NUM_FLOAT:
                if(isdigit(c))
                    strAddChar(&str, (char) c);
                else if((isalpha(c) && (tolower(c) != 'e')) || c == '.'){
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                // Decimal num in exponential form
                else if(tolower(c) == 'e') {
                    strAddChar(&str, (char)c);
                    isDouble = true;
                    stateFlag = NUM_EXPONENT;
                }
                else{
                    ungetc(c, source);
                    tkn.type = NUM_FLOAT;
                    tkn.atr.numberD = strtod(str.str, NULL);
                    LineBegin = false;
                    return tkn;
                }
                break;
            case NUM_EXPONENT:
                if(isdigit(c)){
                    strAddChar(&str, (char)c);
                    stateFlag = NUM_EXPONENT_NUM;
                }
                else if(c == '+'|| c == '-'){
                    strAddChar(&str, (char)c);
                    stateFlag = NUM_EXPONENT_SIGN;
                }
                else{
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                break;
            case NUM_EXPONENT_SIGN:
                if(isdigit(c)){
                    strAddChar(&str, (char)c);
                    stateFlag = NUM_EXPONENT_NUM;
                }
                else{
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                break;
            case NUM_EXPONENT_NUM:
                if(isdigit(c)){
                    strAddChar(&str, (char)c);
                }
                else if(tolower(c) == 'e' || c == '+' || c == '-'){
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                else if(c == '.'){
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
                else{
                    ungetc(c,source);
                    // Return decimal number
                    if(isDouble == true){
                        tkn.type = NUM_FLOAT;
                        tkn.atr.numberD = strtod(str.str, NULL);
                        LineBegin = false;
                        return tkn;
                    }
                    else{
                        // Else return int number
                        tkn.type = NUM;
                        tkn.atr.number = (int)strtod(str.str, NULL);
                        LineBegin = false;
                        return tkn;
                    }
                }
                break;
            case ASSIGNMENT:
                // Equal '=='
                if(c == '='){
                    strAddChar(&str,(char)c);
                    tkn.type = EQUAL;
                    return tkn;
                }
                // Start of block comment
                else if(LineBegin == true){
                    stateFlag = BLOCK_COMMENT;
                    ungetc(c, source);
                    break;
                }
                else{
                    // Assignment
                    ungetc(c,source);
                    tkn.type = ASSIGNMENT;
                    return tkn;
                }
            case MORE_THAN:
                // Operator >=
                if(c == '='){
                    tkn.type = MORE_THAN_EQUAL;
                    return tkn;
                }
                else{
                    // Operator >
                    ungetc(c, source);
                    tkn.type = MORE_THAN;
                    return tkn;
                }
            case LESS_THAN:
                // Operator <=
                if(c == '='){
                    tkn.type = LESS_THAN_EQUAL;
                    return tkn;
                }
                else{
                    // Operator <
                    ungetc(c, source);
                    tkn.type = LESS_THAN;
                    return tkn;
                }
            case NEGATE:
                // Not equal
                if(c == '='){
                    tkn.type = NOT_EQUAL;
                    return tkn;
                }
                else if(c == '(' || isalpha(c)){
                    ungetc(c, source);
                    tkn.type = NEGATE;
                    return tkn;
                }
                else{
                    ungetc(c, source);
                    tkn.type = LEXICAL_ERROR;
                    return tkn;
                }
        }
    }
    tkn.type = EOF;
    return tkn;
}
