#ifndef INCLUDE_COMPENG_H
#define INCLUDE_COMPENG_H

#include "synanlz.h"
#include "vmwrite.h"

typedef struct
{
    Tokenizer *tokenizer;
    VMWriter *vmWriter;
    char currentClassName[100];
    char currentSubroutineName[100];
    int labelCounter;
} CompilationEngine;

void advance(CompilationEngine *compInfo);

void startCompilation(CompilationEngine *compInfo);

void compileClass(CompilationEngine *compInfo);

void compileClassVarDec(CompilationEngine *compInfo);

void compileSubroutine(CompilationEngine *compInfo);

void compileParameterList(CompilationEngine *compInfo);

void compileSubroutineBody(CompilationEngine *compInfo);

void compileVarDec(CompilationEngine *compInfo);

void compileStatements(CompilationEngine *compInfo);

void compileLet(CompilationEngine *compInfo);

void compileIf(CompilationEngine *compInfo);

void compileWhile(CompilationEngine *compInfo);

void compileDo(CompilationEngine *compInfo);

void compileReturn(CompilationEngine *compInfo);

void compileExpression(CompilationEngine *compInfo);

void compileTerm(CompilationEngine *compInfo);

int compileExpressionList(CompilationEngine *compInfo);

void initCompilation(const char *inputFileName);


#endif // INCLUDE_COMPENG_H