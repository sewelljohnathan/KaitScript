#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "datatypes.h"

sym_type checkReservedSyms(char* name);
sym_type checkOtherSyms(int* inputIndex, char* input);


lexeme* generateLexList(char* input) {

    lexeme* lexList = malloc(sizeof(lexeme)*MAX_LEXEME_LIST_LENGTH);
    int lexIndex = 0;
    lexeme curLex;
    int inputIndex = 0;
    char curChar = input[inputIndex];
    int counter = 0;
    int row = 1;

    // Loop through every character
    while (curChar != '\0') {

        /* 
        * I used to have the commented line and don't know why I thought I needed it,
        * so I'm keeping it for future reference just in case shit starts breaking.
        */
        //lexeme curLex = (const struct lexeme) { 0 }
        lexeme curLex;

        if (curChar == '\n') {
            row++;
        }

        curLex.row = row;

        // Comment
        if (curChar == '#') {

            while (1) {

                curChar = input[++inputIndex];
                if (curChar == '\n') {
                    row++;
                    break;
                }
            }

            curChar = input[++inputIndex];
            continue;

        }

        // Whitespace
        else if (iscntrl(curChar) || isspace(curChar)) {
            curChar = input[++inputIndex];
            continue;
        }

        // Indentifier / reserved
        else if (isalpha(curChar)) {

            char name[MAX_VARIABLE_NAME_LENGTH] = "";
            strncat(name, &curChar, 1);

            while (1) {
                curChar = input[++inputIndex];

                // Add to the name
                if (isalnum(curChar)) {
                    strncat(name, &curChar, 1);

                // Figure out the sym_type
                } else {
                    sym_type curSym = checkReservedSyms(name);

                    // This is a variable
                    if (curSym == identsym) {
                        strcpy(curLex.name, name);
                    }

                    // Set the sym
                    curLex.sym = curSym;
                    break;
                }
            }
            
        }

        // Number
        else if (isdigit(curChar)) {

            double number = (double) curChar - '0';
            int foundDecimal = 0;
            double decimalModifier = 0.1;

            while (1) {
                curChar = input[++inputIndex];

                if (isdigit(curChar)) {

                    // Adding digits before the decimal point
                    if (!foundDecimal) {
                        number = number*10 + (curChar - '0');

                    // Add digits after the decimal point
                    } else {
                        number += (curChar - '0')*decimalModifier;
                        decimalModifier *= 0.1;
                    }
                
                // Found the decimal point
                } else if (curChar == '.') {
                    foundDecimal = 1;
                
                } else {
                    curLex.numval = number;
                    curLex.sym = rawnumsym;
                    break;
                }

            }
        }

        // Raw Text
        else if (curChar == '"') {
            
            char text[MAX_RAWTEXT_LENGTH] = "";

            while (1) {
                curChar = input[++inputIndex];
                
                if (curChar != '"') {
                    strncat(text, &curChar, 1);

                } else {
                    strcpy(curLex.textval, text);
                    curLex.sym = rawtextsym;
                    curChar = input[++inputIndex];
                    break;
                }
                

            }
        }

        // Other
        else {

            sym_type curSym = checkOtherSyms(&inputIndex, input);
            curLex.sym = curSym;
            curChar = input[++inputIndex];
        }

        // Add the lexeme to the list
        lexList[lexIndex++] = curLex;
        
    }
    
    // Add an ending lexeme so purely so driver will know when the end is reached
    lexeme endLex = (const struct lexeme) { 0 };
    endLex.sym = -1;
    lexList[lexIndex] = endLex;
    return lexList;

}

sym_type checkReservedSyms(char* name) {

    if (strcmp(name, "num") == 0) {
        return numsym;
    }
    if (strcmp(name, "text") == 0) {
        return textsym;
    }
    if (strcmp(name, "function") == 0) {
        return functionsym;
    }
    if (strcmp(name, "return") == 0) {
        return returnsym;
    }
    if (strcmp(name, "loop") == 0) {
        return loopsym;
    }
    if (strcmp(name, "from") == 0) {
        return fromsym;
    }
    if (strcmp(name, "to") == 0) {
        return tosym;
    }
    if (strcmp(name, "if") == 0) {
        return ifsym;
    }
    if (strcmp(name, "elif") == 0) {
        return elifsym;
    }
    if (strcmp(name, "else") == 0) {
        return elsesym;
    }
    if (strcmp(name, "continue") == 0) {
        return continuesym;
    }
    if (strcmp(name, "break") == 0) {
        return breaksym;
    }

    return identsym;
}

sym_type checkOtherSyms(int* inputIndex, char* input) {
    
    char curChar = input[*inputIndex];
    char nextChar = input[(*inputIndex)+1];
    sym_type curSym;

    switch (curChar) {
        case '=':
            if (nextChar == '=') { curSym = deqsym; (*inputIndex)++; }
            else { curSym = assignsym; }
        break;
        case '!':
            if (nextChar == '=') { curSym = neqsym; (*inputIndex)++; }
        break;
        case '>':
            if (nextChar == '=') { curSym = geqsym; (*inputIndex)++; }
            else { curSym = gtrsym; }
        break;
        case '<':
            if (nextChar == '=') { curSym = leqsym; (*inputIndex)++; }
            else { curSym = lsssym; }
        break;
        case '&':
            if (nextChar == '&') { curSym = andsym; (*inputIndex)++; }
        break;
        case '|':
            if (nextChar == '|') { curSym = orsym; (*inputIndex)++; }
        break;

        case '+': curSym = plussym; break;
        case '-': curSym = subsym; break;
        case '*': curSym = multsym; break;
        case '/': curSym = divsym; break;
        case '%': curSym = modsym; break;
        case '^': curSym = expsym; break;
        
        case '(': curSym = lparensym; break;
        case ')': curSym = rparensym; break;
        case '{': curSym = lbracesym; break;
        case '}': curSym = rbracesym; break;
        case ']': curSym = rbracketsym; break;
        case '[': curSym = lbracketsym; break;

        case ',': curSym = commasym; break;
    }
    
    return curSym;

}