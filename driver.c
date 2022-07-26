#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "datatypes.h"

#define MAX_FILE_LENGTH 10000

const char* symNameList[] = {
    "numsym", "textsym", "identsym", "rawnumsym", "rawtextsym", "assignsym", "functionsym",
    "lparensym", "rparensym", "lbracesym", "rbracesym", "lbracketsym", "rbracketsym",
    "plussym", "subsym", "multsym", "divsym", "expsym",
    "deqsym", "neqsym", "gtrsym", "geqsym", "lsssym", "leqsym",
    "ifsym", "elifsym", "elsesym", "returnsym",
    "commasym",
};

void printHelp();

int main(int argc, char* argv[]) {

    // Just print help if no arguments passed
    if (argc < 2) {
        printHelp();
        return 0;
    }

    char* filename = argv[1];

    // Get the command line input
    int printLexList = 0;
    int printVarTable = 0;
    char cmd;
    while ((cmd = getopt(argc, argv, "hlt")) != -1) {
        switch (cmd) {
            case 'h':
                printHelp();
                return 0;
            break;
            case 'l':
                printLexList = 1;
            break;
            case 't':
                printVarTable = 1;
            break;
        }
    }

    // Open the file
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(argv[1]);
        return 1;
    }

    // Convert the file to a list of char's
    char charList[MAX_FILE_LENGTH];
    int charIndex = 0;
    char curChar;
    while ((curChar = fgetc(fp)) != EOF) {
        charList[charIndex++] = curChar;
        
        if (charIndex == MAX_FILE_LENGTH) {
            printf("File is too large to process.");
            return 1;
        }
    }

    // Get the lexeme list
    lexeme* lexList = generateLexList(charList);

    if (printLexList) {

        int lexIndex = -1;
        lexeme curLex;

        while ((curLex = lexList[++lexIndex]).sym != -1) {
            printf("%s | %s\n", symNameList[curLex.sym], curLex.name);
        }
        printf("\n");
    }

    // Run the interpreter
    return interpretLexList(lexList, printVarTable);

}

void printHelp() {
    printf("\n");
    printf("██╗░░██╗░█████╗░██╗████████╗\n");
    printf("██║░██╔╝██╔══██╗██║╚══██╔══╝\n");
    printf("█████═╝░███████║██║░░░██║░░░\n");
    printf("██╔═██╗░██╔══██║██║░░░██║░░░\n");
    printf("██║░╚██╗██║░░██║██║░░░██║░░░\n");
    printf("╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░░╚═╝░░░\n");
    printf("\n");
    printf("Usage: kait [file] [options]\nOptions:\n");
    printf(" %-10s %s\n", "-h", "Shows this message and exits.");
    printf(" %-10s %s\n", "-l", "Shows the lexeme list generated from the file.");
    printf(" %-10s %s\n", "-h", "Shows var table produced through the execution.");
}

