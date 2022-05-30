#define MAX_VARIABLE_NAME_LENGTH 20
#define MAX_VARIABLE_COUNT 10
#define MAX_RAWTEXT_LENGTH 100
#define MAX_FUNC_PARAMS 10
#define MAX_LEXEME_LIST_LENGTH 100



typedef enum sym_type {
    numsym, textsym, identsym, rawnumsym, rawtextsym, assignsym, functionsym, 
    lparensym, rparensym, lbracesym, rbracesym, lbracketsym, rbracketsym,
    plussym, subsym, mulsym, divsym,
    deqsym, neqsym, gtrsym, geqsym, lsssym, leqsym,
    ifsym, elifsym, elsesym, returnsym,
    commasym,

} sym_type;


typedef enum varType {
    numtype = 1, texttype, nonetype
} varType;


typedef struct lexeme {
    char name[MAX_VARIABLE_NAME_LENGTH];
    double numval;
    char textval[MAX_RAWTEXT_LENGTH];
    sym_type sym;

} lexeme;


typedef struct funcParam {

    char name[MAX_VARIABLE_NAME_LENGTH];
    varType type;

} funcParam;


typedef struct variable {
    char name[MAX_VARIABLE_NAME_LENGTH];
    varType type;
    int isFunc;

    double numVal;
    char textVal[MAX_RAWTEXT_LENGTH];
    int funcStart;
    funcParam funcParams[MAX_FUNC_PARAMS];
    
    int level;

} variable;


lexeme* generateLexList(char* input);
int interpretLexList(lexeme* input, int printVarTableFlag);