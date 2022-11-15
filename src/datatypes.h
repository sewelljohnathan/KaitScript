#define MAX_VARIABLE_NAME_LENGTH 20
#define MAX_VARIABLE_COUNT 100
#define MAX_RAWTEXT_LENGTH 100
#define MAX_FUNC_PARAMS 100
#define MAX_LEXEME_LIST_LENGTH 1000
#define MAX_ARRAY_LENGTH 100
#define MAX_LOOP_ITERATIONS 1000000


typedef enum sym_type {
    numsym = 0, textsym, identsym, rawnumsym, rawtextsym, assignsym, functionsym, 
    loopsym, fromsym, tosym, continuesym, breaksym,
    lparensym, rparensym, lbracesym, rbracesym, lbracketsym, rbracketsym,
    plussym, subsym, multsym, divsym, modsym, expsym,
    deqsym, neqsym, gtrsym, geqsym, lsssym, leqsym,
    ifsym, elifsym, elsesym, andsym, orsym, returnsym,
    commasym,

} sym_type;


typedef enum varType {
    numtype = 0, texttype, nonetype, numArrType, textArrType
} varType;


typedef struct lexeme {
    char name[MAX_VARIABLE_NAME_LENGTH];
    double numval;
    char textval[MAX_RAWTEXT_LENGTH];
    sym_type sym;
    int row;

} lexeme;


typedef struct funcParam {

    char name[MAX_VARIABLE_NAME_LENGTH];
    varType type;

} funcParam;


typedef struct variable {
    char name[MAX_VARIABLE_NAME_LENGTH];
    varType type;
    int isFunc;
    int isArr;
    int arrLength;

    double numVal;
    double numValArr[MAX_ARRAY_LENGTH];
    char textVal[MAX_RAWTEXT_LENGTH];
    char textValArr[MAX_ARRAY_LENGTH][MAX_RAWTEXT_LENGTH];
    int funcStart;
    funcParam funcParams[MAX_FUNC_PARAMS];
    int funcParamsLength;
    
    int level;

} variable;


lexeme* generateLexList(char* input);
int interpretLexList(lexeme* input, int printVarTableFlag);