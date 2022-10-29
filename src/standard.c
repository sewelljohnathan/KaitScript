#include "interpreter.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

void stdPrintNum();
void stdPrintText();
void stdStr();
void stdInt();
int removeTrailingZeroes(double num, char* stream);

int checkStandards(char* name) {

    int inStandards = 0;

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    if (strcmp(name, "printn") == 0) {
        stdPrintNum();
        inStandards = 1;
    }
    if (strcmp(name, "printt") == 0) {
        stdPrintText();
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
    addFuncVar("printn", funcParams, 0, 0);
    addFuncVar("printt", funcParams, 0, 0);
    addFuncVar("str", funcParams, 0, 0);
    addFuncVar("int", funcParams, 0, 0);
}

void stdPrintNum() {

    double arg = numExpression();
    char argText[MAX_RAWTEXT_LENGTH];

    // Remove trailing
    removeTrailingZeroes(arg, argText);

    printf("%s\n", argText);

}

void stdPrintText() {

    char argText[MAX_RAWTEXT_LENGTH];
    strcpy(argText, "");
    textExpression(argText);
    printf("%s\n", argText);

}

void stdStr() {

    double arg = numExpression();
    removeTrailingZeroes(arg, returnText);
    returnType = texttype;

}

void stdInt() {

    char arg[MAX_RAWTEXT_LENGTH];
    textExpression(arg);
    sscanf(arg, "%lf", &returnNum);
    returnType = numtype;

}

void stdRound() {

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