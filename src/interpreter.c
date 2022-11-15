#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void raiseError(lexeme lex, char* msg) {
    printf("%s\nFound \"%d\" on row %d\n", msg, lex.sym, lex.row);
}

int interpretLexList(lexeme* input, int printVarTableFlag) {

    varTable = malloc(sizeof(variable) * MAX_VARIABLE_COUNT);
    varTableIndex = -1;
    varLevel = 0;
    foundContinue = 0;
    foundBreak = 0;
    setStandards();
    
    lexList = input;
    lexIndex = -1;

    while (lexList[lexIndex].sym != -1) {
        if (line()) { return 1; }
    }

    if (printVarTableFlag) {
        printVarTable();
    }

    return 0;
}

int line() {
    
    lexeme firstLex = nextLex();//printf("%d | %d | %s\n", firstLex.sym, firstLex.row, firstLex.name);

    if (firstLex.sym == numsym) {  
        if (handleNumDeclaration()) { return 1; }
    } 
    else if (firstLex.sym == textsym) {
        if (handleTextDeclaration()) { return 1; }
    }
    else if (firstLex.sym == functionsym) {
        if (handleFuncDeclaration()) { return 1; }
    }
    else if (firstLex.sym == identsym) {
        if (handleVarAssignment()) { return 1; }
    } 
    else if (firstLex.sym == loopsym) {
        if (handleLoop()) { return 1; }
    }
    else if (firstLex.sym == ifsym) {
        if (handleIf()) { return 1; }
    }
    else if (firstLex.sym == returnsym) {
        if (handleReturn()) { return 1; }
    }
    else if (firstLex.sym == continuesym) {
        foundContinue = 1;
    }
    else if (firstLex.sym == breaksym) {
        foundBreak = 1;
    }
    else if (firstLex.sym == -1) {
        return 0;
    } else {
        printf("Unexpected %d on line %d", firstLex.sym, firstLex.row);
        return 1;
    }

    return 0;
}

int handleNumDeclaration() {

    // Variable name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { raiseError(identifier, "Expected identifier."); return 1; }

    // Check if this is an array declaration
    int isArr = 0;
    lexeme lbracket = nextLex();
    if (lbracket.sym == lbracketsym) {

        lexeme rbracket = nextLex();
        if (rbracket.sym != rbracketsym) { raiseError(rbracket, "Expected \"]\""); return 1; }

        isArr = 1;
    } else {
        lexIndex--;
    }

    // Assignment '='
    lexeme eqlsign = nextLex();
    if (eqlsign.sym != assignsym) { raiseError(eqlsign, "Expected \"=\"."); return 1; }

    // Create the new variable
    if (isArr) {

        double arr[MAX_ARRAY_LENGTH];
        int arrLength;
        if (numArrExpression(arr, &arrLength)) { return 1; }

        addNumVar(identifier.name, 0, 1);
        for (int i = 0; i < arrLength; i++) {
            varTable[varTableIndex].numValArr[i] = arr[i];
        }
        varTable[varTableIndex].arrLength = arrLength;

    } else {
        double value;
        if (numExpression(&value)) { return 1; }
        addNumVar(identifier.name, value, 0);
    }
    

    return 0;
}

int handleTextDeclaration() {

    // Variable name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { raiseError(identifier, "Expected identifier."); return 1; }

    // Check if this is an array declaration
    int isArr = 0;
    lexeme lbracket = nextLex();
    if (lbracket.sym == lbracketsym) {

        lexeme rbracket = nextLex();
        if (rbracket.sym != rbracketsym) { raiseError(rbracket, "Expected \"]\""); return 1; }

        isArr = 1;
    } else {
        lexIndex--;
    }

    // Assignment '='
    lexeme eqlsign = nextLex();
    if (eqlsign.sym != assignsym) { raiseError(eqlsign, "Expected \"=\"."); return 1; }

    // Create the new variable
    if (isArr) {

        char arr[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH];
        int arrLength;
        if (textArrExpression(arr, &arrLength)) { return 1; }

        addTextVar(identifier.name, "", 1);
        for (int i = 0; i < arrLength; i++) {
            strcpy(varTable[varTableIndex].textValArr[i], arr[i]);
        }
        varTable[varTableIndex].arrLength = arrLength;

    } else {

        char text[MAX_RAWTEXT_LENGTH] = "";
        if (textExpression(text)) { return 1; }
        addTextVar(identifier.name, text, 0);
    }



    return 0;
}

int handleFuncDeclaration() {

    // Function name
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { raiseError(identifier, "Expected identifier."); return 1; }

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "Expected \"(\"."); return 1; }

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
        if (paramIdentifier.sym != identsym) { raiseError(paramIdentifier, "Expected identifier."); return 1; }  

        int isArr = 0;
        lexeme lbracket = nextLex();
        if (lbracket.sym == lbracketsym) {

            lexeme rbracket = nextLex();
            if (rbracket.sym != rbracketsym) { raiseError(rbracket, "Expected \"]\""); return 1; }

            isArr = 1;
        } else {
            lexIndex--;
        }

        // Add the param to the list
        funcParam* curParam = &funcParams[funcParamLength++];
        strcpy(curParam->name, paramIdentifier.name);

        if (paramType.sym == numsym) {
            if (isArr) {
                curParam->type = numArrType;
            } else {
                curParam->type = numtype;
            }
        } else if (paramType.sym == textsym) {
            if (isArr) {
                curParam->type = textArrType;
            } else {
                curParam->type = texttype;
            }
        } else {
            raiseError(paramType, "Expected \"num\" or \"text\"");
            return 1;
        }
        
        // Comma or ')'
        lexeme paramEnd = nextLex();
        if (paramEnd.sym == commasym) { 
            // Get the new param type
            paramType = nextLex();

        } else if (paramEnd.sym != rparensym) {
            raiseError(paramEnd, "Expected \")\".");
            return 1;

        } else {
            break;
        }
    }

    // Get the func type
    lexeme next = nextLex();
    varType funcType;

    // Check if it is an array
    int isArr = 0;
    lexeme lbracket = nextLex();
    if (lbracket.sym == lbracketsym) {

        lexeme rbracket = nextLex();
        if (rbracket.sym != rbracketsym) { raiseError(rbracket, "Expected \"]\""); return 1; }

        isArr = 1;
    } else {
        lexIndex--;
    }

    // Set the func type
    if (next.sym == numsym) {
        if (isArr) {
            funcType = numArrType;
        } else {
            funcType = numtype;
        }
    } else if (next.sym == textsym) {
        if (isArr) {
            funcType = textArrType;
        } else {
            funcType = texttype;
        }
    }

    // This check is in case there is no type listed (i.e. a nonetype)
    if (next.sym != lbracesym) {
        // Opening Brace
        lexeme lbrace = nextLex();
        if (lbrace.sym != lbracesym) { raiseError(lbrace, "Expected \"{\"."); return 1; }
    }

    // Add the function to the table
    addFuncVar(identifier.name, funcParams, funcParamLength, lexIndex+1, funcType);
    
    // Pass through all of the rest of the function
    int nestedLevel = 1;
    next = nextLex();
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
    
    return 0;
}

int handleVarAssignment() {

    // Variable name
    lexeme identifier = lexList[lexIndex];

    // Assign the value
    int tableIndex = findVar(identifier.name);
    variable curVar = varTable[tableIndex];

    if (!curVar.isFunc) {

        if (curVar.isArr) {
            
            // Opening brakcet
            lexeme lbracket = nextLex();
            if (lbracket.sym == assignsym) {
                
                // Assign 
                if (curVar.type == numArrType) {
                    double arr[MAX_ARRAY_LENGTH];
                    int arrLength;
                    if (numArrExpression(arr, &arrLength)) { return 1; }

                    for (int i = 0; i < arrLength; i++) {
                        varTable[tableIndex].numValArr[i] = arr[i];
                    }
                    varTable[tableIndex].arrLength = arrLength;

                } else if (curVar.type == textArrType) {
                    char arr[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH];
                    int arrLength;
                    if (textArrExpression(arr, &arrLength)) { return 1; }

                    for (int i = 0; i < arrLength; i++) {
                        strcpy(varTable[tableIndex].textValArr[i], arr[i]);
                    }
                    varTable[tableIndex].arrLength = arrLength;
                }

                return 0;
            }
            if (lbracket.sym != lbracketsym) { raiseError(lbracket, "Expected \"[\""); return 1; }

            // Find the index
            int arrIndex;
            double arrIndexInput;
            if (numExpression(&arrIndexInput)) { return 1; }
            if (floor(arrIndexInput) != ceil(arrIndexInput)) { printf("Expected an integer"); return 1; }
            arrIndex = (int) (arrIndexInput);

            // Closing bracket
            lexeme rbracket = nextLex();
            if (rbracket.sym != rbracketsym) { raiseError(rbracket, "Expected \"]\""); return 1; }

            // Assignment '='
            lexeme eqlsign = nextLex();
            if (eqlsign.sym != assignsym) { raiseError(eqlsign, "Expected \"=\"."); return 1; }

            // number
            if (curVar.type == numArrType) {
                double value;
                if (numExpression(&value)) { return 1; }
                varTable[tableIndex].numValArr[arrIndex] = value;

            // text
            } else if (curVar.type == textArrType) {
                char text[MAX_RAWTEXT_LENGTH] = "";
                if (textExpression(text)) { return 1; }
                strcpy(varTable[tableIndex].textValArr[arrIndex], text);

            }

        } else {

            // Assignment '='
            lexeme eqlsign = nextLex();
            if (eqlsign.sym != assignsym) { raiseError(eqlsign, "Expected \"=\"."); return 1; }

            // number
            if (curVar.type == numtype) {
                double value;
                if (numExpression(&value)) { return 1; }
                varTable[tableIndex].numVal = value;

            // text
            } else if (curVar.type == texttype) {
                char text[MAX_RAWTEXT_LENGTH] = "";
                if (textExpression(text)) { return 1; }
                strcpy(varTable[tableIndex].textVal, text);

            }
        } 

    } else {
        if (handleFuncCall(identifier)) { return 1; }
    }

    return 0;
}

int handleFuncCall(lexeme identifier) {

    // Get the table entry
    int tableIndex = findVar(identifier.name);
    variable funcVar = varTable[tableIndex];
    returnType = funcVar.type;

    switch (checkStandards(identifier.name)) {
        case 1: return 1; break;
        case -1: return 0; break;
        // case 0: keep going, was not in standards
    }

    // Opening Paren
    lexeme lparen = nextLex();
    if (lparen.sym != lparensym) { raiseError(lparen, "Expected \"(\"."); return 1; }

    // Loop through all the arguments
    varLevel++;
    for (int i = 0; i < funcVar.funcParamsLength; i++) {

        funcParam currentParam = funcVar.funcParams[i];

        // Copy the arg values into new variables with the param names
        if (currentParam.type == numtype) {
            double argVal;
            if (numExpression(&argVal)) { return 1; }
            addNumVar(currentParam.name, argVal, 0);

        } else if (currentParam.type == texttype) {
            char argVal[MAX_RAWTEXT_LENGTH] = "";
            if (textExpression(argVal)) { return 1; }
            addTextVar(currentParam.name, argVal, 0);
        
        } else if (currentParam.type == numArrType) {

            double argument[MAX_ARRAY_LENGTH];
            int argumentLength;
            if (numArrExpression(argument, &argumentLength)) { return 1; }

            // Copy the array values
            addNumVar(currentParam.name, 0, 1);
            for (int i = 0; i < argumentLength; i++) {
                varTable[varTableIndex].numValArr[i] = argument[i];
            }

        } else if (currentParam.type == textArrType) {

            char argument[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH];
            int argumentLength;
            if (textArrExpression(argument, &argumentLength)) { return 1; }

            // Copy the array values
            addTextVar(currentParam.name, "", 1);
            for (int i = 0; i < argumentLength; i++) {
                strcpy(varTable[varTableIndex].textValArr[i], argument[i]);
            }
        }

        // Check if the next symbol is a command, or a ')' if this is the last arg
        lexeme commaOrRparen = nextLex();
        if (!(
            (i < funcVar.funcParamsLength-1 && commaOrRparen.sym == commasym) ||
            (i == funcVar.funcParamsLength-1 && commaOrRparen.sym == rparensym)
        )) {
            raiseError(commaOrRparen, "Should be a \",\" or \")\"");
            return 1;
        }
    }

    // Make sure you get the closing ) if no args
    if (funcVar.funcParamsLength == 0) {
        lexeme rparen = nextLex();
        if (rparen.sym != rparensym) { raiseError(rparen, "Expected \")\"."); return 1; }
    }

    int oldIndex = lexIndex;
    // Run the function
    lexIndex = funcVar.funcStart-1;
    while (lexList[lexIndex+1].sym != rbracesym) {
        if (line()) { return 1; };
    }

    // }
    lexeme rbrace = nextLex();
    if (rbrace.sym != rbracesym) { raiseError(rbrace, "Expected \"}\"."); return 1; }

    markVars();
    varLevel--;
    lexIndex = oldIndex;
    
    return 0;
}

int handleLoop() {

    varLevel++;

    // Loop keyword
    lexeme loop = lexList[lexIndex];
    int start, end;

    // counter variable
    lexeme identifier = nextLex();
    if (identifier.sym != identsym) { raiseError(identifier, "Expected identifier."); return 1; }

    addNumVar(identifier.name, 0, 0);
    int tableIndex = varTableIndex;

    // From keyword
    lexeme from = nextLex();

    // Normal
    if (from.sym == fromsym) {

        // Get the start number
        double startInput;
        if (numExpression(&startInput)) { return 1; }
        if (floor(startInput) != ceil(startInput)) {
            printf("Expected an integer."); return 1;
        } else {
            start = (int) (startInput);
        }

        // To keyword
        lexeme to = nextLex();

        // Get the end number
        double endInput;
        if (numExpression(&endInput)) { return 1; }
        if (floor(endInput) != ceil(endInput)) {
            printf("Expected an integer"); return 1;
        } else {
            end = (int) (endInput);
        }

        // {
        lexeme lbrace = nextLex();
        if (lbrace.sym != lbracesym) { raiseError(lbrace, "Expected \"{\"."); return 1; }
    
    // If this is an infinite loop
    } else if (from.sym == lbracesym) {
        start = 0;
        end = MAX_LOOP_ITERATIONS;
    } else {
        raiseError(from, "Expected \"from\" or \"{\"."); return 1;
    }

    // Check if there are not too many iterations
    if (end - start > MAX_LOOP_ITERATIONS) {
        printf("Too many loop iterations attempted."); return 1;
    }


    // Do the loop
    int startLexIndex = lexIndex;
    for (int i = start; i <= end; i++) {

        // Increase the counter variable
        varTable[tableIndex].numVal = i;

        // Do the loop
        lexIndex = startLexIndex;
        while (lexList[lexIndex+1].sym != rbracesym) {
            if (line()) { return 1; }
            
            // Check if a continue/break was found
            if (foundContinue || foundBreak) {
                break;
            }
        }

        // If there was a break, end here
        foundContinue = 0;
        if (foundBreak) {
            break;
        }
        
    }
    foundBreak = 0;

    /*
    Go back to the start and eat the entire loop.
    If a continue is the seen in the last loop iteration, then problems were caused.
    Additionally, this is easier than trying to know how to jump to the end after a break
    */
    if (1) {
        lexIndex = startLexIndex;

        int nestedLevel = 1;
        lexeme next = nextLex();
        while (1) {

            if (next.sym == lbracesym) { nestedLevel++; }
            if (next.sym == rbracesym) { nestedLevel--; }
            if (next.sym == rbracesym && nestedLevel == 0) { break; }
            next = nextLex();
        }
        lexIndex--;
    }
    
    // }
    lexeme rbrace = nextLex();
    if (rbrace.sym != rbracesym) { raiseError(rbrace, "Expected \"}\"."); return 1; }

    markVars();
    varLevel--;
    
    return 0;
}

int handleIf() {

    int seenElse = 0;
    int foundTrue = 0;
    lexeme curLex;

    while (1) {
        
        int isTrue;
        if (!seenElse) {

            curLex = nextLex();
            
            // Determine if this is a mathematical or string conditional
            int isNum;
            if (curLex.sym == rawnumsym) {
                isNum = 1;
            } else if (curLex.sym == rawtextsym) {
                isNum = 0;
            } else if (curLex.sym == identsym) {

                int tableIndex = findVar(curLex.name);
                variable curVar = varTable[tableIndex];

                if (curVar.type == numtype || curVar.type == numArrType) {
                    isNum = 1;
                } else if (curVar.type == texttype || curVar.type == textArrType) {
                    isNum = 0;
                }
            }

            lexIndex--;
            if (isNum) {

                // Get the values
                double result1;
                double result2;
                if (numExpression(&result1)) { return 1; }
                lexeme conditionalSwitch = nextLex();
                if (numExpression(&result2)) { return 1; }
                
                // check the conditional
                switch (conditionalSwitch.sym) {
                    case deqsym: isTrue = result1 == result2; break;
                    case neqsym: isTrue = result1 != result2; break;
                    case gtrsym: isTrue = result1 > result2; break;
                    case geqsym: isTrue = result1 >= result2; break;
                    case lsssym: isTrue = result1 < result2; break;
                    case leqsym: isTrue = result1 <= result2; break;
                    default:
                        raiseError(conditionalSwitch, "Expected a conditional switch"); 
                        return 1; 
                    break;
                }

            } else {

                // Get the values
                char text1[MAX_RAWTEXT_LENGTH];
                char text2[MAX_RAWTEXT_LENGTH];
                if (textExpression(text1)) { return 1; }
                lexeme conditionalSwitch = nextLex();
                if (textExpression(text2)) { return 1; }

                // check the conditional
                switch (conditionalSwitch.sym) {
                    case deqsym: isTrue = strcmp(text1, text2) == 0; break;
                    case neqsym: isTrue = strcmp(text1, text2) != 0; break;
                    default: 
                        raiseError(conditionalSwitch, "Expected a conditional switch"); 
                        return 1; 
                    break;
                }

            }

        // If this is the else clause, always run
        } else {
            isTrue = 1;
        }
        // Eat the '{'
        curLex = nextLex();
        if (curLex.sym != lbracesym) { raiseError(curLex, "Expected \"{\""); return 1; }

        // Run the conditional code
        if (isTrue && !foundTrue) {
            foundTrue = 1;
            
            while (lexList[lexIndex+1].sym != rbracesym) {
                if (line()) { return 1; }
            }

        // Just eat code
        } else {

            int nestedLevel = 1;
            lexeme next = nextLex();
            while (1) {

                if (next.sym == lbracesym) { nestedLevel++; }
                if (next.sym == rbracesym) { nestedLevel--; }
                if (next.sym == rbracesym && nestedLevel == 0) { break; }
                next = nextLex();
            }
            lexIndex--;
        }

        // Eat the '}'
        curLex = nextLex();
        if (curLex.sym != rbracesym) { raiseError(curLex, "Expected \"}\""); return 1; }

        // Break if there is not another if clause
        curLex = nextLex();

        // Handle else
        if (curLex.sym == elsesym) {
            if (seenElse) { raiseError(curLex, "Cannot have multiple \"else\" clauses"); }
            else { seenElse = 1; }
        }

        // Do another
        if (!(curLex.sym == elifsym || curLex.sym == elsesym)) {
            break;
        }

    }

    lexIndex--;
    return 0;
}

int handleReturn() {

    if (returnType == numtype) {
        if (numExpression(&returnNum)) { return 1; }
    } else if (returnType == texttype) {
        if (textExpression(returnText)) { return 1; }
    } else if (returnType == numArrType) {
        if (numArrExpression(returnNumArr, &returnArrLength)) { return 1; }
    } else if (returnType == textArrType) {
        if (textArrExpression(returnTextArr, &returnArrLength)) { return 1; }
    }
}

int numExpression(double* num) {

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
            if (!isOperator(lexList[lexIndex-1]) && pastFirst == 1) {
                raiseError(curLex, "Unexpected \"(\"");
                return 1;
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
                return 1;
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
                return 1;
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
                return 1;
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
                lexList[lexIndex-1].sym == rparensym ||
                lexList[lexIndex-1].sym == rbracketsym) {
                break;
            }

            int tableIndex = findVar(curLex.name);
            variable curVar = varTable[tableIndex];

            if (curVar.isFunc) {
                if (handleFuncCall(curLex)) { return 1; }

                // Hacky; force the return value into a new lex
                lexeme insertLex;
                insertLex.sym = rawnumsym;
                insertLex.numval = returnNum;

                shuntingOutput[++shuntingOutputIndex] = insertLex;
            }
            else {
                
                // If an array, hacky inject a lex
                if (curVar.isArr) { 

                    lexeme arrLbracket = nextLex();
                    if (arrLbracket.sym != lbracketsym) { raiseError(arrLbracket, "Expected \"[\""); return 1; }

                    // Get the index
                    int arrIndex;
                    double arrIndexInput;
                    if (numExpression(&arrIndexInput)) { return 1; }
                    if (floor(arrIndexInput) != ceil(arrIndexInput)) { printf("Expected an integer."); return 1; }
                    arrIndex = (int) (arrIndexInput);

                    lexeme arrRbracket = nextLex();
                    if (arrRbracket.sym != rbracketsym) { raiseError(arrRbracket, "Expected \"]\""); return 1; }

                    // Insert
                    lexeme insertLex;
                    insertLex.sym = rawnumsym;
                    insertLex.numval = curVar.numValArr[arrIndex];

                    shuntingOutput[++shuntingOutputIndex] = insertLex;
                    
                } else {
                    shuntingOutput[++shuntingOutputIndex] = curLex;
                }
            }

            if (negateNum) { 
                shuntingOutput[shuntingOutputIndex].numval *= -1;
                negateNum = 0;
            }
        } else {
            break;
        }

        if (curLex.sym != lparensym) { pastFirst = 1; }
        curLex = nextLex();

    }
    if (isOperator(lexList[lexIndex-1])) {
        raiseError(lexList[lexIndex-1], "Expected a number.");
        return 1;
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
                    if (prevValue2 < 0 && floor(prevValue1) != ceil(prevValue1)) {
                        printf("Cannot take fractional exponents of negative numbers.\nFound at row %d.\n", nextOutput.row);
                        return 1;
                    }
                    newValue = pow(prevValue2, prevValue1);
                break;
            }
            valueStack[--valueStackIndex] = newValue;

        }

    }

    *num = valueStack[0];

    return 0;
}

int numArrExpression(double* arr, int* length) {
    
    int arrLength = 0;

    lexeme curLex;

    // Loop through the expression
    int expectPlus = 0;
    while (1) {

        curLex = nextLex();

        // Check if plus
        if (curLex.sym == plussym) {
            if (expectPlus) {
                expectPlus = 0;
                continue;
            } else {
                raiseError(curLex, "Expected an array"); 
                return 1;
            }
        }

        // Nothing left
        if (curLex.sym != plussym && expectPlus) {
            break;
        }

        // Variable
        if (curLex.sym == identsym) {
            int tableIndex = findVar(curLex.name);
            variable curVar = varTable[tableIndex];

            // Return a function
            if (curVar.isFunc) {
                if (handleFuncCall(curLex)) { return 1; }   
                for (int i = 0; i < returnArrLength; i++) {
                    arr[arrLength++] = returnNumArr[i];
                }
            
            // Variable[]
            } else {
                for (int i = 0; i < curVar.arrLength; i++) {
                    arr[arrLength++] = curVar.numValArr[i];
                }
            }

            expectPlus = 1;
            continue;
        }

        // subarr
        else if (curLex.sym == lbracketsym) {

            while (1) {

                curLex = nextLex();
                if (curLex.sym == rbracketsym) {
                    break;
                }
                // Revert 1 for numExpression
                lexIndex--;

                // Get the next
                double nextValue;
                if (numExpression(&nextValue)) { return 1; }
                arr[arrLength++] = nextValue;

                // Get the ',' or ']'
                curLex = nextLex();
                if (curLex.sym == commasym) {
                    continue;
                } else if (curLex.sym == rbracketsym) {
                    break;
                } else {
                    raiseError(curLex, "Expected \",\" or \"]\""); return 1;
                }
            }

            expectPlus = 1;
        }
    }
    *length = arrLength;

    lexIndex--;
    
    return 0;
}

int textExpression(char* text) {

    int seenFirstSym = 0;
    char bufferText[MAX_RAWTEXT_LENGTH] = "";

    lexeme curLex = nextLex();
    while (1) {

        // Do nothing if there is a plus sign (appending is done below)
        // Check if this is an illegal plus sign at the start
        if (curLex.sym == plussym) {
            if (seenFirstSym == 0) {
                raiseError(curLex, "Expected a number.");
                return 1;
            }

        } else if (curLex.sym == rawtextsym) {
            // Check if there is no plus sign before.
            if (seenFirstSym == 1 && lexList[lexIndex-1].sym != plussym) {
                break;
            }
            strcat(bufferText, curLex.textval);
            seenFirstSym = 1;

        } else if (curLex.sym == rawnumsym) {
            if (seenFirstSym == 1 && lexList[lexIndex-1].sym != plussym) {
                break;
            }
            convertNumToText(curLex.numval, curLex.textval);
            strcat(bufferText, curLex.textval);
            seenFirstSym = 1;

        } else if (curLex.sym == identsym) {
            if (seenFirstSym && lexList[lexIndex-1].sym != plussym) {
                break;
            }
            int tableIndex = findVar(curLex.name);
            variable curVar = varTable[tableIndex];

            if (curVar.isFunc) {
                if (handleFuncCall(curLex)) { return 1; }
                strcat(bufferText, returnText);
            } else {
                
                if (curVar.isArr) {

                    lexeme arrLbracket = nextLex();
                    if (arrLbracket.sym != lbracketsym) { raiseError(arrLbracket, "Expected \"[\""); return 1; }

                    // Get the index
                    int arrIndex;
                    double arrIndexInput;
                    if (numExpression(&arrIndexInput)) { return 1; }
                    if (floor(arrIndexInput) != ceil(arrIndexInput)) { printf("Expected an integer."); return 1; }
                    arrIndex = (int) (arrIndexInput);

                    lexeme arrRbracket = nextLex();
                    if (arrRbracket.sym != rbracketsym) { raiseError(arrRbracket, "Expected \"]\""); return 1; }

                    // Copy the value in text
                    
                    if (curVar.type == numArrType) {
                        convertNumToText(curVar.numValArr[arrIndex], curVar.textVal);
                    } else if (curVar.type == textArrType) {
                        strcpy(curVar.textVal, curVar.textValArr[arrIndex]);
                    }
                } else {

                    // If the var is a number, hacky convert it
                    if (curVar.type == numtype) {
                        convertNumToText(curVar.numVal, curVar.textVal);
                    }
                }
                    
                strcat(bufferText, curVar.textVal);
            }

            seenFirstSym = 1;

        } else {
            break;
        }

        curLex = nextLex();

    }
    strcpy(text, bufferText);
    lexIndex--;

    return 0;
}

int textArrExpression(char arr[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH], int* length) {
    
    int arrLength = 0;

    lexeme curLex;

    // Loop through the expression
    int expectPlus = 0;
    while (1) {

        curLex = nextLex();

        // Check if plus
        if (curLex.sym == plussym) {
            if (expectPlus) {
                expectPlus = 0;
                continue;
            } else {
                raiseError(curLex, "Expected an array"); 
                return 1;
            }
        }

        // Nothing left
        if (curLex.sym != plussym && expectPlus) {
            break;
        }

        // Variable
        if (curLex.sym == identsym) {
            int tableIndex = findVar(curLex.name);
            variable curVar = varTable[tableIndex];

            // Return a function
            if (curVar.isFunc) {
                if (handleFuncCall(curLex)) { return 1; }   
                for (int i = 0; i < returnArrLength; i++) {
                    strcpy(arr[arrLength++], returnTextArr[i]);
                }
            
            // Variable[]
            } else {
                for (int i = 0; i < curVar.arrLength; i++) {
                    strcpy(arr[arrLength++], curVar.textValArr[i]);
                }
            }

            expectPlus = 1;
            continue;
        }

        // subarr
        else if (curLex.sym == lbracketsym) {

            while (1) {

                curLex = nextLex();
                if (curLex.sym == rbracketsym) {
                    break;
                }
                // Revert 1 for numExpression
                lexIndex--;

                // Get the next
                char nextValue[MAX_RAWTEXT_LENGTH];
                if (textExpression(nextValue)) { return 1; }
                strcpy(arr[arrLength++], nextValue);

                // Get the ',' or ']'
                curLex = nextLex();
                if (curLex.sym == commasym) {
                    continue;
                } else if (curLex.sym == rbracketsym) {
                    break;
                } else {
                    raiseError(curLex, "Expected \",\" or \"]\""); return 1;
                }
            }

            expectPlus = 1;
        }
    }
    *length = arrLength;

    lexIndex--;
    
    return 0;
}
