#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void raiseError(lexeme lex, char* msg) {
    printf("%s\nFound \"%d\" on row %d\n", msg, lex.sym, lex.row);
    exit(1);
}

int interpretLexList(lexeme* input, int printVarTableFlag) {

    varTable = malloc(sizeof(variable) * MAX_VARIABLE_COUNT);
    varTableIndex = -1;
    varLevel = 0;
    setStandards();
    
    lexList = input;
    lexIndex = -1;

    while (lexList[lexIndex].sym != -1) {
        line();
    }

    if (printVarTableFlag) {
        printVarTable();
    }

    return 0;
}

void line() {
    
    lexeme firstLex = nextLex();//printf("%d | %d | %s\n", firstLex.sym, firstLex.row, firstLex.name);

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
    else if (firstLex.sym == loopsym) {
        handleLoop();
    }
    else if (firstLex.sym == returnsym) {
        handleReturn();
    }
    else if (firstLex.sym == -1) {
        return;
    } else {
        raiseError(firstLex, "Unexpected token.");
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
    char text[MAX_RAWTEXT_LENGTH] = "";
    textExpression(text);
    addTextVar(identifier.name, text);

}

void handleFuncDeclaration() {

    // Function name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { raiseError(identifier, "Not an identifier"); }

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    // Initialize some values
    funcParam funcParams[MAX_FUNC_PARAMS];
    int funcParamLength = 0;

    // Get the list of function parameters
    lexeme paramType = nextLex();
    int isParams = 1;
    if (paramType.sym == rparensym) { isParams = 0; }
    while (isParams) {
        
        // Param name
        lexeme paramIdentifier = nextLex();
        if (paramIdentifier.sym != identsym) { raiseError(paramIdentifier, "Not an identifier"); }       

        // Add the param to the list
        funcParam* curParam = &funcParams[funcParamLength++];
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
            raiseError(paramEnd, "No closing \")\"");

        } else {
            break;
        }
    }

    // Add the function to the table
    addFuncVar(identifier.name, funcParams, funcParamLength, lexIndex+2);

    // Pass through all of the rest of the function
    int nestedLevel = 0;
    lexeme next = nextLex();
    while (1) {

        if (next.sym == lbracesym) {
            nestedLevel++;
        }
        if (next.sym == rbracesym) {
            nestedLevel--;
        }
        if (next.sym == rbracesym && nestedLevel == 0) {
            break;
        }

        next = nextLex();
    }
    
}

void handleVarAssignment() {

    // Variable name
    lexeme identifier = lexList[lexIndex];

    // Assign the value
    int tableIndex = findVar(identifier.name);
    variable curVar = varTable[tableIndex];

    if (!curVar.isFunc) {

        // Assignment '='
        lexeme eqlsign = nextLex();
        if (eqlsign.sym != assignsym) { return; }

        if (curVar.type == numtype) {
            double value = numExpression();
            varTable[tableIndex].numVal = value;
        } else if (curVar.type == texttype) {
            char text[MAX_RAWTEXT_LENGTH] = "";
            textExpression(text);
            strcpy(varTable[tableIndex].textVal, text);
        }
    } else {
        handleFuncCall(identifier);
    }

}

void handleFuncCall(lexeme identifier) {

    if (checkStandards(identifier.name)) {
        return;
    }

    // Get the table entry
    int tableIndex = findVar(identifier.name);
    variable funcVar = varTable[tableIndex];

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "No opening \"(\""); }

    // Loop through all the arguments
    varLevel++;
    for (int i = 0; i < funcVar.funcParamsLength; i++) {
        
        lexeme arg = nextLex();

        // Copy the arg values into new variables with the param names
        if (funcVar.funcParams[i].type == numtype) {
            addNumVar(funcVar.funcParams[i].name, arg.numval);

        } else if (funcVar.funcParams[i].type == texttype) {
            addTextVar(funcVar.funcParams[i].name, arg.textval);
        
        }

        // Check if the next symbol is a command, or a ')' if this is the last arg
        lexeme commaOrRparen = nextLex();
        if (!(
            (i < funcVar.funcParamsLength-1 && commaOrRparen.sym == commasym) ||
            (i == funcVar.funcParamsLength-1 && commaOrRparen.sym == rparensym)
        )) {
            raiseError(commaOrRparen, "Should be a \",\" or \")\"");
        }
    }

    // Make sure you get the closing ) if no args
    if (funcVar.funcParamsLength == 0) {
        lexeme rparen = nextLex();
        if (rparen.sym != rparensym) { raiseError(rparen, "No closing \")\""); }
    }

    int oldIndex = lexIndex;
    // Run the function
    lexIndex = funcVar.funcStart-1;
    while (lexList[lexIndex+1].sym != rbracesym) {
        line();
    }

    // }
    lexeme rbrace = nextLex();
    if (rbrace.sym != rbracesym) { raiseError(rbrace, "No closing \"}\""); }

    markVars();
    varLevel--;
    lexIndex = oldIndex;
    
}

void handleLoop() {

    varLevel++;

    // Loop keyword
    lexeme loop = lexList[lexIndex];
    int start, end;

    // counter variable
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { return; }

    addNumVar(identifier.name, 0);
    int tableIndex = varTableIndex;

    // From keyword
    lexeme from = nextLex();

    // Get the start number
    start = numExpression();

    // To keyword
    lexeme to = nextLex();

    // Get the end number
    end = numExpression();
   
    // {
    lexeme lbrace = nextLex();
    if (lbrace.sym != lbracesym) { return; }

    // Do the loop
    int startLexIndex = lexIndex;
    for (int i = start; i <= end; i++) {

        // Increase the counter variable
        varTable[tableIndex].numVal = i;

        // Do the loop
        lexIndex = startLexIndex;
        while (lexList[lexIndex+1].sym != rbracesym) {
            line();
        }
        
    }
    
    // }
    lexeme rbrace = nextLex();
    if (rbrace.sym != rbracesym) { return; }

    markVars();
    varLevel--;
    
}

void handleReturn() {

    unknownExpression(&returnNum, returnText, &returnType);
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
    int seenLparen = 0;

    curLex = nextLex();
    while (1) {
        
        // '('
        if (curLex.sym == lparensym) {
            seenLparen++;
            
            // Grammer Check
            // Previous must be an operator or the first lex
            if (!isOperator(lexList[lexIndex-1]) && pastFirst == 0) {
                raiseError(curLex, "Unexpected \"(\"");
            }

            shuntingStack[++shuntingStackIndex] = curLex;

        // ')'
        } else if (curLex.sym == rparensym) {

            if (seenLparen == 0) {
                break;
            }
            seenLparen--;

            // Grammer Check
            // Previous cannot be an operator
            if (isOperator(lexList[lexIndex-1])) {
                raiseError(curLex, "Unexpected \")\"");
            }

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
            
            // Grammer Check
            // Previous cannot be an operator unless this is a negator, and cannot be a '('
            if (
                !isNegator(lexIndex, pastFirst) &&
                (lexList[lexIndex-1].sym == lparensym || isOperator(lexList[lexIndex-1]))
             ) {
                raiseError(curLex, "Unexpected operator.");
            }

            // https://stackoverflow.com/questions/46861254/infix-to-postfix-for-negative-numbers
            if (isNegator(lexIndex, pastFirst)) {

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

            // Grammer Check
            // Previous cannot be a number
            if (lexList[lexIndex-1].sym == rawnumsym || lexList[lexIndex-1].sym == identsym) {
                raiseError(curLex, "Unexpected number.");
            }

            shuntingOutput[++shuntingOutputIndex] = curLex;

            if (negateNum) { 
                shuntingOutput[shuntingOutputIndex].numval *= -1;
                negateNum = 0;
            }
    
        } else if (curLex.sym == identsym) {

            // Grammer Check
            // Previous cannot be a number
            if (
                lexList[lexIndex-1].sym == rawnumsym || 
                lexList[lexIndex-1].sym == identsym || 
                lexList[lexIndex-1].sym == rparensym) {
                break;
            }

            int tableIndex = findVar(curLex.name);
            variable curVar = varTable[tableIndex];

            if (curVar.isFunc) {
                handleFuncCall(curLex);

                // Hacky; force the return value into a new lex
                lexeme insertLex;
                insertLex.sym = rawnumsym;
                insertLex.numval = returnNum;

                shuntingOutput[++shuntingOutputIndex] = insertLex;
            }
            else {
                shuntingOutput[++shuntingOutputIndex] = curLex;
            }

            if (negateNum) { 
                shuntingOutput[shuntingOutputIndex].numval *= -1;
                negateNum = 0;
            }
        } else {
            break;
        }

        pastFirst = 1;
        curLex = nextLex();

    }
    if (isOperator(lexList[lexIndex-1])) {
        raiseError(lexList[lexIndex-1], "Expected a number.");
    }
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

void textExpression(char* text) {

    int seenFirstSym = 0;

    lexeme curLex = nextLex();
    while (1) {

        // Do nothing if there is a plus sign (appending is done below)
        // Check if this is an illegal plus sign at the start
        if (curLex.sym == plussym) {
            if (seenFirstSym == 0) {
                raiseError(curLex, "Expected a number.");
            }

        } else if (curLex.sym == rawtextsym) {
            // Check if there is no plus sign before.
            if (seenFirstSym == 1 && lexList[lexIndex-1].sym != plussym) {
                break;
            }
            strcat(text, curLex.textval);
            seenFirstSym = 1;

        } else if (curLex.sym == identsym) {
            if (seenFirstSym == 1 && lexList[lexIndex-1].sym != plussym) {
                break;
            }
            int tableIndex = findVar(curLex.name);
            variable curVar = varTable[tableIndex];

            if (curVar.isFunc) {
                handleFuncCall(curLex);
                strcat(text, returnText);
            } else {
                strcat(text, curVar.textVal);
            }

            seenFirstSym = 1;

        } else {
            break;
        }

        curLex = nextLex();

    }
    lexIndex--;

}
