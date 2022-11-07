#include "datatypes.h"

variable* varTable;
int varTableIndex;
int varLevel;

double returnNum;
char returnText[MAX_RAWTEXT_LENGTH];
varType returnType;

lexeme* lexList;
int lexIndex;

// interpreter main
void raiseError(lexeme lex, char* msg);
void line();
void handleNumDeclaration();
void handleTextDeclaration();
void handleFuncDeclaration();
void handleVarAssignment();
void handleFuncCall(lexeme identifier);
void handleLoop();
void numExpression(double* num);
void textExpression(char* text);

// interpreter tools
int findVar(char* name);
void markVars();
void printVarTable();
lexeme nextLex();
void addNumVar(char* name, double value);
void addTextVar(char* name, char* text);
void addFuncVar(char* name, funcParam* funcParams, int funcParamLength, int funcStart, varType funcType);
int isOperator(lexeme lex);
int operatorPrecedence(lexeme lex);
int isNegator(int index, int pastFirst);
int convertNumToText(double num, char* stream);

// standards
void setStandards();
int checkStandards(char* name);