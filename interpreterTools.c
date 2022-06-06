#include "interpreter.h"
#include <string.h>
#include <stdio.h>

int findVar(char* name) {

    for (int i = varTableIndex; i >= 0; i--) {
    
        if (strcmp(varTable[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

void markVars() {

    for (int i = varTableIndex; i >= 0; i--) {

        variable* curVar = &varTable[i];
        if (curVar->level == varLevel-1) {
            break;
        }

        varTableIndex--;
    }
}

void printVarTable() {

    printf("Variable Table\n");
    printf("%-10s | %-6s | %-6s | %-6s | %-7s | %-s\n", "Name", "Type", "isFunc", "numVal", "textVal", "funcStart");
    printf("=============================================================\n");
    for (int i = 0; i < varTableIndex+1; i++) {
        variable curVar = varTable[i];
        printf(
            "%-10s | %-6d | %-6d | %-6lf | %-7s | %-d\n", 
            curVar.name, 
            curVar.type,
            curVar.isFunc,
            curVar.numVal, 
            curVar.textVal, 
            curVar.funcStart
        );
    }

}

lexeme nextLex() {
    return lexList[++lexIndex];
}

void addNumVar(char* name, double value) {

    variable* curVar = &(varTable[++varTableIndex]);
    strcpy(curVar->name, name);
    curVar->type = numtype;
    curVar->isFunc = 0;
    curVar->numVal = value;
}

void addTextVar(char* name, char* text) {

    variable* curVar = &varTable[++varTableIndex];
    strcpy(curVar->name, name);
    curVar->type = texttype;
    curVar->isFunc = 0;
    strcpy(curVar->textVal, text);
}

void addFuncVar(char* name, funcParam* funcParams, int funcParamLength, int funcStart, varType type) {

    variable* curVar = &varTable[++varTableIndex];
    strcpy(curVar->name, name);
    curVar->type = type;
    curVar->isFunc = 1;

    for (int i = 0; i < funcParamLength; i++) {
        curVar->funcParams[i] = funcParams[i];
    }
    curVar->funcStart = funcStart;
}

int isOperator(lexeme lex) {
    switch (lex.sym) {
        case plussym: return 1; break;
        case subsym: return 1; break;
        case multsym: return 1; break;
        case divsym: return 1; break;
        case expsym: return 1; break;
        default: return 0; break;
    }
}
int operatorPrecedence(lexeme lex) {
    switch (lex.sym) {
        case plussym: return 1; break;
        case subsym: return 1; break;
        case multsym: return 2; break;
        case divsym: return 2; break;
        case expsym: return 3; break;
        case lparensym: return 0; break;
        default: return -1; break;
    }
}
int isNegator(int index, int pastFirst) {
    lexeme curLex = lexList[index];
    lexeme prevLex = lexList[index-1];
    if (curLex.sym == subsym && (isOperator(prevLex) || prevLex.sym == lparensym || pastFirst == 0)) {
        return 1;
    }
    return 0;
}