#include "interpreter.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

void stdPrint();
void stdStr();
void stdInt();
int removeTrailingZeroes(double num, char* stream);

int checkStandards(char* name) {

    if (strcmp(name, "print") == 0) {
        stdPrint(1); // Temporary decision to only have println. May revert later
        return 1;
    }
    if (strcmp(name, "println") == 0) {
        stdPrint(1);
        return 1;
    }
    if (strcmp(name, "str") == 0) {
        stdStr();
        return 1;
    }
    if (strcmp(name, "int") == 0) {
        stdInt();
        return 1;
    }

    return 0;

}

void setStandards() {

    funcParam funcParams[1];
    addFuncVar("print", funcParams, 0, 0);
    addFuncVar("println", funcParams, 0, 0);
    addFuncVar("str", funcParams, 0, 0);
    addFuncVar("int", funcParams, 0, 0);
}

void stdPrint(int lineFlag) {

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

    // Remove trailing
    if (argType == numtype) {
        removeTrailingZeroes(argNum, argText);
    }

    // Print
    if (lineFlag) {
        printf("%s\n", argText);
    } else {
        printf("%s", argText);
    }

}

void stdStr() {

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    double arg = numExpression();
    removeTrailingZeroes(arg, returnText);
    returnType = texttype;

    // Closing Paren
    lexeme rparen = nextLex();
    if (rparen.sym != rparensym) { raiseError(lparen, "No closing \")\""); }
}

void stdInt() {

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    char arg[MAX_RAWTEXT_LENGTH];
    textExpression(arg);
    sscanf(arg, "%lf", &returnNum);
    returnType = numtype;
    
    // Closing Paren
    lexeme rparen = nextLex();
    if (rparen.sym != rparensym) { raiseError(lparen, "No closing \")\""); }
}

int removeTrailingZeroes(double num, char* stream) {

    // Find how many to remove
    int placement = -1;
    for (int i = 6; i >= 0; i--) {
        if (num*pow(10.0, i) - (int) (num*pow(10.0, i)) != 0) {
            placement = i+1;
        }
    }
    if (placement == -1) { placement = 0; }

    // Format to remove
    switch (placement) {
        case 0: sprintf(stream, "%.0lf", num); break;
        case 1: sprintf(stream, "%.1lf", num); break;
        case 2: sprintf(stream, "%.2lf", num); break;
        case 3: sprintf(stream, "%.3lf", num); break;
        case 4: sprintf(stream, "%.4lf", num); break;
        case 5: sprintf(stream, "%.5lf", num); break;
        case 6: sprintf(stream, "%.6lf", num); break;
        case 7: sprintf(stream, "%.6lf", num); break;
    }

}