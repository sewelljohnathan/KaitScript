#include "interpreter.h"
#include <string.h>
#include <stdio.h>

int stdPrint();
int stdInput();
int stdStr();
int stdInt();

int checkStandards(char* name) {

    int inStandards = 0;

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    if (strcmp(name, "print") == 0) {
        if (stdPrint()) { return 1; }
        inStandards = 1;
    }
    if (strcmp(name, "input") == 0) {
        if (stdInput()) { return 1; }
        inStandards = 1;
    }
    if (strcmp(name, "str") == 0) {
        if (stdStr()) { return 1; }
        inStandards = 1;
    }
    if (strcmp(name, "int") == 0) {
        if (stdInt()) { return 1; }
        inStandards = 1;
    }

    // TODO: move to handleFuncCall
    if (inStandards) {
        // Closing Paren
        lexeme rparen = nextLex();
        if (rparen.sym != rparensym) { raiseError(rparen, "No closing \")\""); return 1; }
    
    } else {
        lexIndex--;
    }

    if (inStandards) {
        return -1;
    } else {
        return 0;
    }

}

void setStandards() {

    funcParam funcParams[1];
    addFuncVar("print", funcParams, 0, 0, nonetype);
    addFuncVar("input", funcParams, 0, 0, texttype);
    addFuncVar("str", funcParams, 0, 0, texttype);
    addFuncVar("int", funcParams, 0, 0, numtype);
}

int stdPrint() {

    double argNum;
    char argText[MAX_RAWTEXT_LENGTH] = "";
    if (textExpression(argText)) { return 1; }
    printf("%s\n", argText);

    return 0;
}

int stdInput() {

    double argNum;
    char argText[MAX_RAWTEXT_LENGTH] = "";
    if (textExpression(argText)) { return 1; }
    printf(": %s", argText);

    char responseText[MAX_RAWTEXT_LENGTH] = "";
    fgets(responseText, sizeof(responseText), stdin);

    // Get rid of newline
    responseText[strcspn(responseText, "\r\n")] = '\0';

    strcpy(returnText, responseText);

    return 0;
}

int stdStr() {

    double arg;
    if (numExpression(&arg)) { return 1; }
    convertNumToText(arg, returnText);
    returnType = texttype;

    return 0;
}

int stdInt() {

    char arg[MAX_RAWTEXT_LENGTH] = "";
    if (textExpression(arg)) { return 1; }
    sscanf(arg, "%lf", &returnNum);
    returnType = numtype;

    return 0;
}
