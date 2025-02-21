#ifndef INCLUDE_VMCODETABLE_H
#define INCLUDE_VMCODETABLE_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vmtrack.h"

// Keeping track of occurences
int eqCount = 0; // counting eq commands
int gtCount = 0; // counting gt commands
int ltCount = 0; // counting lt commands

int retCount = 0; // counting return labels


void bootStrapCode(FILE *outputFile)
{
    fprintf(outputFile, "@256\nD=A\n@SP\nM=D\n");
    fprintf(outputFile, "@Sys.init\n0;JMP\n");
}

// CONSTANT TYPE COMMANDS
void pushConst(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", num);
}

// PUSH ARGUMENT N
void pushArg(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@ARG\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", num);
}

// POP ARGUMENT N
void popArg(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@ARG\nD=M+D\n@R15\nM=D\n@SP\nAM=M-1\nD=M\n@R15\nA=M\nM=D\n", num);
}

// PUSH LOCAL N
void pushLcl(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@LCL\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", num);
}

// POP LOCAL N
void popLcl(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@LCL\nD=M+D\n@R15\nM=D\n@SP\nAM=M-1\nD=M\n@R15\nA=M\nM=D\n", num);
}

// PUSH POINTER 1/0
void pushPtr(FILE *outputFile, const char *num)
{
    if (strcmp(num, "0") == 0)
        fprintf(outputFile, "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
    else
        fprintf(outputFile, "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
}

// POP POINTER 1/0
void popPtr(FILE *outputFile, const char *num)
{
    if (strcmp(num, "0") == 0)
        fprintf(outputFile, "@SP\nAM=M-1\nD=M\n@THIS\nM=D\n");
    else
        fprintf(outputFile, "@SP\nAM=M-1\nD=M\n@THAT\nM=D\n");
}

// PUSH THIS N
void pushThis(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@THIS\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", num);
}

// POP THIS N
void popThis(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@THIS\nD=M+D\n@R15\nM=D\n@SP\nAM=M-1\nD=M\n@R15\nA=M\nM=D\n", num);
}

// PUSH THAT N
void pushThat(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@THAT\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", num);
}

// POP THAT N
void popThat(FILE *outputFile, const char *num)
{
    fprintf(outputFile, "@%s\nD=A\n@THAT\nD=M+D\n@R15\nM=D\n@SP\nAM=M-1\nD=M\n@R15\nA=M\nM=D\n", num);
}

// PUSH TEMP N
void pushTemp(FILE *outputFile, const char *num)
{
    int intNum = atoi(num) + 5;
    fprintf(outputFile, "@%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", intNum);
}

// POP TEMP N
void popTemp(FILE *outputFile, const char *num)
{
    int intNum = atoi(num) + 5;
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\n@%d\nM=D\n", intNum);
}

// PUSH STATIC N
void pushStat(FILE *outputFile, const char *num)
{
    const char *fileName = getCurFileName();
    fprintf(outputFile, "@%s.%s\nM=D\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fileName, num);
}

// POP STATIC N
void popStat(FILE *outputFile, const char *num)
{
    const char *fileName = getCurFileName();
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\n@%s.%s\nM=D\n", fileName, num);
}

// ADDTION OPERATION
void addOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nM=M+D\n");
}

// SUBTRACTION OPERATION
void subOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nM=M-D\n");
}

// NEGATION OPERATION
void negOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nA=M-1\nM=-M\n");
}

// AND OPERATION
void andOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nM=M&D\n");
}

// OR OPERATION
void orOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nM=M|D\n");
}

// NOT OPERATION
void notOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nA=M-1\nM=!M\n");
}

// (IF EQUAL TO) OPERATION
void eqOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\nM=0\n@END_EQ_%d\nD;JNE\n@SP\nA=M-1\nM=-1\n(END_EQ_%d)\n", eqCount, eqCount);
}

// (IF GREATER THAN) OPERATION
void gtOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\nM=0\n@END_GT_%d\nD;JLE\n@SP\nA=M-1\nM=-1\n(END_GT_%d)\n", gtCount, gtCount);
}

// (IF LESS THAN) OPERATION
void ltOp(FILE *outputFile)
{
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\nM=0\n@END_LT_%d\nD;JGE\n@SP\nA=M-1\nM=-1\n(END_LT_%d)\n", ltCount, ltCount);
}

void gotoCommand(FILE *outputFile, char *label)
{
    const char *fileName = getCurFileName();
    const char *funcName = getCurFuncName();

    fprintf(outputFile, "@%s$%s\n0;JMP\n", funcName, label);
}

void ifgotoCommand(FILE *outputFile, char *label)
{
    const char *fileName = getCurFileName();
    const char *funcName = getCurFuncName();

    fprintf(outputFile, "@SP\nAM=M-1\nD=M\nA=A-1\n@%s$%s\nD;JNE\n", funcName, label);
}

void labelCommand(FILE *outputFile, char *label)
{
    const char *fileName = getCurFileName();
    const char *funcName = getCurFuncName();

    fprintf(outputFile, "(%s$%s)\n", funcName, label);
}

void callCommand(FILE *outputFile, char *funcName, char *nArgs)
{

    const char *fileName = getCurFileName();

    // push return address
    fprintf(outputFile, "@RET_ADDRESS_CALL%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", retCount);

    // Save LCL, ARG, THIS, THAT
    const char *segments[] = {"LCL", "ARG", "THIS", "THAT"};
    for (int i = 0; i < 4; i++)
        fprintf(outputFile, "@%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", segments[i]);

    // Reposition ARG and LCL
    fprintf(outputFile, "@SP\nD=M\n@5\nD=D-A\n@%s\nD=D-A\n@ARG\nM=D\n", nArgs);
    fprintf(outputFile, "@SP\nD=M\n@LCL\nM=D\n");

    // Jump to the function
    fprintf(outputFile, "@%s\n0;JMP\n", funcName);

    // Declare return label
    fprintf(outputFile, "(RET_ADDRESS_CALL%d)\n", retCount);
}


void functionCommand(FILE *outputFile, char *funcName, char *nVars)
{
    pushFunc(funcName);
    const char *fileName = getCurFileName();

    // Inject the function entry label
    fprintf(outputFile, "(%s)\n", funcName);    

    // push nVars 0's to the stack for local variables
    int nTime = atoi(nVars);
    for(int i = 0; i < nTime; i++)
        fprintf(outputFile, "@SP\nA=M\nM=0\n@SP\nM=M+1\n");
}


void returnCommand(FILE *outputFile)
{
    // save the frame and return address
    fprintf (outputFile, "@LCL\nD=M\n@R13\nM=D\n");         // frame(R13) = LCL
    fprintf (outputFile, "@5\nA=D-A\nD=M\n@R14\nM=D\n");    // retAddress(R14) = *(frame - 5)

    // reposition the return value for the caller
    fprintf(outputFile, "@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\n");

    // reposition SP for the caller
    fprintf(outputFile, "@ARG\nD=M+1\n@SP\nM=D\n");

    // restore THAT, THIS, ARG, LCL
    const char *segments[] = {"THAT", "THIS", "ARG", "LCL"};
    for (int i = 0; i < 4; i++)
        fprintf(outputFile, "@R13\nD=M-1\nAM=D\nD=M\n@%s\nM=D\n", segments[i]);

    // Jump to the return address which was stored in R14
    fprintf(outputFile, "@R14\nA=M\n0;JMP\n");
}

#endif // INCLUDE_VMCODETABLE_H