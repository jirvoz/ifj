#ifndef _INST_LIST_H
#define _INST_LIST_H

//enum of instructions
typedef enum instruction 
{
//frames, function calling
    MOVE_INST,
    CREATEFRAME_INST,
    PUSTFRAME_INST,
    POPFRAME_INST,
    DEFVAR_INST,
    CALL_INST,
    RETURN_INST,
//stack
    PUSHS_INST,
    POPS_INST,
    CLEARS_INST,
//aritmetic, relational, bool and convert instructions
    ADD_INST,
    SUB_INST,
    MUL_INST,
    DIV_INST,
    ADDS_INST,
    SUBS_INST,
    MULS_INST,
    DIVS_INST,
    LT_INST,
    GT_INST,
    EQ_INST,
    LTS_INST,
    GTS_INST,
    EQS_INST,
    AND_INST,
    OR_INST,
    NOT_INST,
    ANDS_INST,
    ORS_INST,
    NOTS_INST,
    INT2FLOAT_INST,
    FLOAT3INT_INST,
    FLOAT2R2EINT_INST,
    FLOAT2R20INT_INST,
    INT2CHAR_INST,
    STRI2INT_INST,
    INT2FLOATS_INST,
    FLOAT2INTS_INST,
    FLOAT2R2EINTS_INST,
    FLOAT2R20INTS_INST,
    INT2CHARS_INST,
    STRI2INTS_INST,
//input_INST,output 
    READ_INST,
    WRITE_INST,
//strings
    CONCAT_INST,
    STRLEN_INST,
    GETCHAR_INST,
    SETCHAR_INST,
//types
    TYPE_INST,
//program flow
    LABEL_INST,
    JUMP_INST,
    JUMPIFEQ_INST,
    JUMPIFNEQ_INST,
    JUMPIFEQS_INST,
    JUMPIFNEQS_INST,
//tune instructions
    BREAK_INST,
    DPRINT_INST
} instruction;

typedef struct tInst 
{
    instruction instType;    //type of instructions
    void* addr1;              //adress 1
    void* addr2;              //adress 2
    void* addr3;              //adress 3
} tInst;

typedef struct tListItem 
{
    tInst instruction;
    struct tListItem* nextItem;
} tListItem;
    
typedef struct tlistOfInst 
{
    tListItem* first;         //pointer to first item
    tListItem* last;           //pointer to last item
    tListItem* active;        //pointer to active item
} tListOfInst;

void listInit(tListOfInst* list);
void listFree(tListOfInst* list);
void listInsertLast(tListOfInst* list, tInst instruction);
void listFirst(tListOfInst* list);
void listNext(tListOfInst* list);
void listGoto(tListOfInst* list, void* gotoInst);
void* listGetPointerLast(tListOfInst *list);
tInst* listGetData(tListOfInst* list);

#endif
