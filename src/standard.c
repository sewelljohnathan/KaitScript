#include "interpreter.h"
#include <string.h>
#include <stdio.h>

void stdPrint();
void stdInput();
void stdStr();
void stdInt();
void stdRound();

int checkStandards(char* name) {

    int inStandards = 0;

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    if (strcmp(name, "print") == 0) {
        stdPrint();
        inStandards = 1;
    }
    if (strcmp(name, "input") == 0) {
        stdInput();
        inStandards = 1;
    }
    if (strcmp(name, "str") == 0) {
        stdStr();
        inStandards = 1;
    }
    if (strcmp(name, "int") == 0) {
        stdInt();
        inStandards = 1;
    }

    // TODO: move to handleFuncCall
    if (inStandards) {
        // Closing Paren
        lexeme rparen = nextLex();
        if (rparen.sym != rparensym) { raiseError(rparen, "No closing \")\""); }
    
    } else {
        lexIndex--;
    }

    return inStandards;

}

void setStandards() {

    funcParam funcParams[1];
    addFuncVar("print", funcParams, 0, 0, texttype);
    addFuncVar("input", funcParams, 0, 0, texttype);
    addFuncVar("str", funcParams, 0, 0, texttype);
    addFuncVar("int", funcParams, 0, 0, numtype);
}

void stdPrint() {

    double argNum;
    char argText[MAX_RAWTEXT_LENGTH] = "";
    textExpression(argText);
    printf("%s\n", argText);

}

void stdInput() {

    double argNum;
    char argText[MAX_RAWTEXT_LENGTH] = "";
    textExpression(argText);
    printf("%s", argText);

    char responseText[MAX_RAWTEXT_LENGTH] = "";
    fgets(responseText, sizeof(responseText), stdin);

    strcpy(returnText, responseText);

}

void stdStr() {

    double arg = numExpression();
    convertNumToText(arg, returnText);
    returnType = texttype;

}

void stdInt() {

    char arg[MAX_RAWTEXT_LENGTH] = "";
    textExpression(arg);
    sscanf(arg, "%lf", &returnNum);
    returnType = numtype;

}

void stdRound() {

}
