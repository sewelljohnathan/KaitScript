#include "datatypes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

variable* varTable;
int varTableIndex;
int varLevel;

lexeme* lexList;
int lexIndex;

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


int interpretLexList(lexeme* input, int printVarTableFlag) {

    varTable = malloc(sizeof(variable) * MAX_VARIABLE_COUNT);
    varTableIndex = -1;
    varLevel = 0;
    
    lexList = input;
    lexIndex = -1;

    while (lexList[lexIndex].sym != -1) {

        line(lexList, &lexIndex);
    }

    if (printVarTableFlag) {
        printVarTable();
    }

    return 0;
}

void addNumVar(char* name, double value) {

    variable* curVar = &(varTable[++varTableIndex]);
    strcpy(curVar->name, name);
    curVar->type = numtype;
    curVar->isFunc = 0;
    curVar->numVal = value;
}

void addTextVar(char* name, char* text) {

    variable* curVar = &varTable[++varTableIndex];
    strcpy(curVar->name, name);
    curVar->type = texttype;
    curVar->isFunc = 0;
    strcpy(curVar->textVal, text);
}

void addFuncVar(char* name, funcParam* funcParams, int funcParamLength, int funcStart, varType type) {

    variable* curVar = &varTable[++varTableIndex];
    strcpy(curVar->name, name);
    curVar->type = type;
    curVar->isFunc = 1;

    for (int i = 0; i < funcParamLength; i++) {
        curVar->funcParams[i] = funcParams[i];
    }
    curVar->funcStart = funcStart;
}

int findVar(char* name) {

    for (int i = varTableIndex; i >= 0; i--) {
    
        if (strcmp(varTable[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

void markVars() {

    for (int i = varTableIndex; i >= 0; i--) {

        variable* curVar = &varTable[i];
        if (curVar->level == varLevel-1) {
            break;
        }

        varTableIndex--;
    }
}

void printVarTable() {

    printf("Variable Table\n");
    printf("%-10s | %-6s | %-6s | %-6s | %-7s | %-s\n", "Name", "Type", "isFunc", "numVal", "textVal", "funcStart");
    printf("=============================================================\n");
    for (int i = 0; i < varTableIndex+1; i++) {
        variable curVar = varTable[i];
        printf(
            "%-10s | %-6d | %-6d | %-6lf | %-7s | %-d\n", 
            curVar.name, 
            curVar.type,
            curVar.isFunc,
            curVar.numVal, 
            curVar.textVal, 
            curVar.funcStart
        );
    }

}

lexeme nextLex() {
    return lexList[++lexIndex];
}

void line() {
    
    lexeme firstLex = nextLex();

    if (firstLex.sym == numsym) {  
        handleNumDeclaration();
    } 
    else if (firstLex.sym == textsym) {
        handleTextDeclaration();
    }
    else if (firstLex.sym == functionsym) {
        handleFuncDeclaration();
    }
    else if (firstLex.sym == identsym) {
        handleVarAssignment();
    }


}

void handleNumDeclaration() {

    // Variable name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { return; }

    // Assignment '='
    lexeme eqlsign = nextLex();
    if (eqlsign.sym != assignsym) { return; }

    // Create the new variable
    double value = numExpression();
    addNumVar(identifier.name, value);

}

void handleTextDeclaration() {

    // Variable name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { return; }

    // Assignment '='
    lexeme eqlsign = nextLex();
    if (eqlsign.sym != assignsym) { return; }

    // Create the new variable
    addTextVar(identifier.name, textExpression());

}

void handleFuncDeclaration() {

    // Function name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { return; }

    // Openign Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { return; }

    // Initialize some values
    funcParam funcParams[MAX_FUNC_PARAMS];
    int funcParamLength = -1;

    // Get the list of function parameters
    lexeme paramType = nextLex();
    while (paramType.sym != rparensym) {
        
        // Param name
        lexeme paramIdentifier = nextLex();
        if (paramIdentifier.sym != identsym) { return; }       

        // Add the param to the list
        funcParam* curParam = &funcParams[++funcParamLength];
        strcpy(curParam->name, paramIdentifier.name);

        if (paramType.sym == numsym) {
            curParam->type = numtype;
        } else if (paramType.sym == textsym) {
            curParam->type = texttype;
        } else {
            // Error
            return;
        }
        
        // Comma or ')'
        lexeme paramEnd = nextLex();
        if (paramEnd.sym == commasym) { 
            // Get the new param type
            paramType = nextLex();

        } else if (paramEnd.sym != rparensym) {
            // Error
            return;

        }
    }

    lexeme funcType = nextLex();
    varType type;
    if (funcType.sym == numsym) {
        type = numtype;
    } else if (funcType.sym == textsym) {
        type = texttype;
    } else if (funcType.sym == lbracesym) {
        type = nonetype;
        lexIndex--;
    } else {
        // ERROR
    }

    // The opening brace of the function
    lexeme lbrace = nextLex();
    if (lbrace.sym != lbracesym) { return; }

    // Add the function to the table
    addFuncVar(identifier.name, funcParams, funcParamLength, lexIndex, type);

}

void handleVarAssignment() {

    // Variable name
    lexeme identifier = lexList[lexIndex];

    // Assignment '='
    lexeme eqlsign = nextLex();
    if (eqlsign.sym != assignsym) { return; }

    // Assign the value
    int tableIndex = findVar(identifier.name);
    variable curVar = varTable[tableIndex];

    if (curVar.type == numtype) {
        double value = numExpression();
        varTable[tableIndex].numVal = value;
    } else if (curVar.type == texttype) {
        char* text = textExpression();
        strcpy(varTable[tableIndex].textVal, text);
    }
}

double numExpression() {

    lexeme curLex;

    // Shunting yard, modified to allow for negation
    lexeme* shuntingOutput = malloc(sizeof(lexeme)*50);
    int shuntingOutputIndex = -1;
    lexeme* shuntingStack = malloc(sizeof(lexeme)*50);
    int shuntingStackIndex = -1;

    int addSubAfterParen = 0;
    int negateNum = 0;
    int pastFirst = 0;

    curLex = nextLex();
    do {
        
        // '('
        if (curLex.sym == lparensym) {
            shuntingStack[++shuntingStackIndex] = curLex;

        // ')'
        } else if (curLex.sym == rparensym) {

            // Pop operators until '(' is found
            while (1) {

                if (shuntingStack[shuntingStackIndex].sym == lparensym) {
                    break;
                }

                shuntingOutput[++shuntingOutputIndex] = shuntingStack[shuntingStackIndex];
                shuntingStackIndex--;
            }
            shuntingStackIndex--;

            if (addSubAfterParen > 0) {
                lexeme subber;
                subber.sym = subsym;
                shuntingOutput[++shuntingOutputIndex] = subber;

                addSubAfterParen--;
            }

        } else if (isOperator(curLex)) {

            lexeme prevLex = lexList[lexIndex-1];
            // https://stackoverflow.com/questions/46861254/infix-to-postfix-for-negative-numbers
            if (curLex.sym == subsym && (isOperator(prevLex) || prevLex.sym == lparensym || pastFirst == 0)) {

                lexeme afterLex = lexList[lexIndex+1];

                // If the next lex is a variable, simply negate it later
                if (afterLex.sym == identsym || afterLex.sym == rawnumsym) {
                    negateNum = 1;
                
                // If the next lex is '(', then prepare the negation
                } else if (afterLex.sym == lparensym) {
                    lexeme zero;
                    zero.sym = rawnumsym;
                    zero.numval = 0;

                    shuntingOutput[++shuntingOutputIndex] = zero;
        
                    addSubAfterParen++;
                }
            
            } else {
                // Pop operators until a higher one is found
                while (operatorPrecedence(shuntingStack[shuntingStackIndex]) >= operatorPrecedence(curLex)) {

                    shuntingOutput[++shuntingOutputIndex] = shuntingStack[shuntingStackIndex];
                    shuntingStackIndex--;
                }

                // Add the new one to the stack
                shuntingStack[++shuntingStackIndex] = curLex;
            }  
        
        } else if (curLex.sym == rawnumsym) {

            shuntingOutput[++shuntingOutputIndex] = curLex;

            if (negateNum) { 
                shuntingOutput[shuntingOutputIndex].numval *= -1;
                negateNum = 0;
            }
    
        } else if (curLex.sym == identsym) {
            shuntingOutput[++shuntingOutputIndex] = curLex;

            if (negateNum) { 
                shuntingOutput[shuntingOutputIndex].numval *= -1;
                negateNum = 0;
            }
        }

        pastFirst = 1;
        curLex = nextLex();

    } while (isOperator(curLex) || curLex.sym == lparensym || curLex.sym == rparensym || curLex.sym == identsym || curLex.sym == rawnumsym);
    lexIndex--;

    // Push the remaining operators onto the stack
    while (shuntingStackIndex >= 0) {
        shuntingOutput[++shuntingOutputIndex] = shuntingStack[shuntingStackIndex];
        shuntingStackIndex--;
    }

    /*
    //STACK DEBUGGING
    for (int i = 0; i <= shuntingOutputIndex; i++) {

        lexeme lex = shuntingOutput[i];
        if (isOperator(lex)) {
            printf("%d\n", lex.sym);
        } else {
            printf("%lf | %d\n", lex.numval, lex.sym);
        }
    }
    */

    // Postfix evaluation
    double* valueStack = malloc(sizeof(double)*50);
    int valueStackIndex = -1;

    for (int i = 0; i <= shuntingOutputIndex; i++) {

        lexeme nextOutput = shuntingOutput[i];

        if (nextOutput.sym == rawnumsym) {
            valueStack[++valueStackIndex] = nextOutput.numval;

        } else if (nextOutput.sym == identsym) {
            int tableIndex = findVar(nextOutput.name);
            valueStack[++valueStackIndex] = varTable[tableIndex].numVal;

        } else if (isOperator(nextOutput)) {

            double prevValue1 = valueStack[valueStackIndex];
            double prevValue2 = valueStack[valueStackIndex-1];
            double newValue;
            switch (nextOutput.sym) {
                case plussym: newValue = prevValue2 + prevValue1; break;
                case subsym: newValue = prevValue2 - prevValue1; break;
                case multsym: newValue = prevValue2 * prevValue1; break;
                case divsym: newValue = prevValue2 / prevValue1; break;
                case expsym: 
                    newValue = prevValue2;
                    for (int i = 1; i < prevValue1; i++) {
                        newValue *= prevValue2;
                    }
                break;
            }
            valueStack[--valueStackIndex] = newValue;

        }

    }

    return valueStack[0];

}


char* textExpression() {

}

double runFuncNum() {

}

char* runFuncText() {

}

void runFuncNone() {

}

int isOperator(lexeme lex) {
    switch (lex.sym) {
        case plussym: return 1; break;
        case subsym: return 1; break;
        case multsym: return 1; break;
        case divsym: return 1; break;
        case expsym: return 1; break;
        default: return 0; break;
    }
}
int operatorPrecedence(lexeme lex) {
    switch (lex.sym) {
        case plussym: return 1; break;
        case subsym: return 1; break;
        case multsym: return 2; break;
        case divsym: return 2; break;
        case expsym: return 3; break;
        case lparensym: return 0; break;
        default: return -1; break;
    }
}