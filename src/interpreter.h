#include "datatypes.h"

variable* varTable;
int varTableIndex;
int varLevel;

double returnNum;
char returnText[MAX_RAWTEXT_LENGTH];
varType returnType;

lexeme* lexList;
int lexIndex;

void line();
void addNumVar(char* name, double value);
void addTextVar(char* name, char* text);
void addFuncVar(char* name, funcParam* funcParams, int funcParamLength, int funcStart, varType funcType);
int findVar(char* name);
void markVars();
void printVarTable();
void handleNumDeclaration();
void handleTextDeclaration();
void handleFuncDeclaration();
void handleVarAssignment();
void handleFuncCall(lexeme identifier);
void handleLoop();
double numExpression();
void textExpression();
lexeme nextLex();
int isOperator(lexeme lex);
int operatorPrecedence(lexeme lex);
int isNegator(int index, int pastFirst);
void raiseError(lexeme lex, char* msg);

void unknownExpression(double* num, char* text, varType* expressionType);
int convertNumToText(double num, char* stream);
void setStandards();
int checkStandards(char* name);