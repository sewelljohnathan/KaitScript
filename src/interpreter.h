#include "datatypes.h"

variable* varTable;
int varTableIndex;
int varLevel;

double returnNum;
double returnNumArr[MAX_ARRAY_LENGTH];
char returnText[MAX_RAWTEXT_LENGTH];
char returnTextArr[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH];
int returnArrLength;
varType returnType;

lexeme* lexList;
int lexIndex;

// interpreter main
void raiseError(lexeme lex, char* msg);
int line();
int handleNumDeclaration();
int handleTextDeclaration();
int handleFuncDeclaration();
int handleVarAssignment();
int handleFuncCall(lexeme identifier);
int handleLoop();
int handleReturn();
int numExpression(double* num);
int numArrExpression(double* arr, int* length);
int textExpression(char* text);
int textArrExpression(char arr[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH], int* length);

// interpreter tools
lexeme nextLex();
int findVar(char* name);
void markVars();
void printVarTable();
void addNumVar(char* name, double value, int isArr);
void addTextVar(char* name, char* text, int isArr);
void addFuncVar(char* name, funcParam* funcParams, int funcParamLength, int funcStart, varType funcType);
int isOperator(lexeme lex);
int operatorPrecedence(lexeme lex);
int isNegator(int index, int pastFirst);
void convertNumToText(double num, char* stream);

// standards
void setStandards();
int checkStandards(char* name);