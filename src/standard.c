#include "interpreter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void handlePrint();

int checkStandards(char* name) {

    if (strcmp(name, "print") == 0) {
        handlePrint(0);
        return 1;
    }
    if (strcmp(name, "println") == 0) {
        handlePrint(1);
        return 1;
    }

    return 0;

}

void setStandards() {

    funcParam funcParams[1];
    addFuncVar("print", funcParams, 0, 0);
    addFuncVar("println", funcParams, 0, 0);
}

void handlePrint(int lineFlag) {

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
        if (lineFlag) {
            printf("%lf\n", argNum);
        } else {
            printf("%lf", argNum);
        }
    }
    if (argType == texttype) {
        if (lineFlag) {
            printf("%s\n", argText);
        } else {
            printf("%s", argText);
        }
    }
}