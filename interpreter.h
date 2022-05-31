#include "datatypes.h"

void line();
void addNumVar(char* name, double value);
void addTextVar(char* name, char* text);
void addFuncVar(char* name, funcParam* funcParams, int funcParamLength, int funcStart, varType type);
int findVar(char* name);
void markVars();
void printVarTable();
void handleNumDeclaration();
void handleTextDeclaration();
void handleFuncDeclaration();
void handleVarAssignment();
double numExpression();
char* textExpression();
double runFuncNum();
char* runFuncText();
void runFuncNone();
lexeme nextLex();
int isOperator(lexeme lex);
int operatorPrecedence(lexeme lex);