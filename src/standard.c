#include "interpreter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void handlePrint();
void handlePrintln();

int checkStandards(char* name) {

    if (strcmp(name, "print") == 0) {
        handlePrint();
        return 1;
    }
    if (strcmp(name, "println") == 0) {
        handlePrintln();
        return 1;
    }

    return 0;

}

void setStandards() {

    funcParam funcParams[1];
    addFuncVar("print", funcParams, 0, 0);
    addFuncVar("println", funcParams, 0, 0);
}

void handlePrint() {

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    double argNum;
    char argText[MAX_RAWTEXT_LENGTH];
    varType argType;
    unknownExpression(&argNum, argText, &argType);

    // Closing Paren
    lexeme rparen = nextLex();
    if (rparen.sym != rparensym) { raiseError(lparen, "No closing \")\""); }

    if (argType == numtype) {
        printf("%lf", argNum);
    }
    if (argType == texttype) {
        printf("%s", argText);
    }
}
void handlePrintln() {

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    double argNum;
    char argText[MAX_RAWTEXT_LENGTH];
    varType argType;
    unknownExpression(&argNum, argText, &argType);

    // Closing Paren
    lexeme rparen = nextLex();
    if (rparen.sym != rparensym) { raiseError(lparen, "No closing \")\""); }

    if (argType == numtype) {
        printf("%lf\n", argNum);
    }
    if (argType == texttype) {
        printf("%s\n", argText);
    }
}