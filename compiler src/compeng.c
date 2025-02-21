#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "compeng.h"
#include "symtbl.h"

// Reads the next token(if exists), and writes it
void advance(CompilationEngine *compInfo)
{
    getNextToken(compInfo->tokenizer);
}

void startCompilation(CompilationEngine *compInfo)
{
    advance(compInfo);
    compileClass(compInfo); // Entry point
}

// CLASS ----> 'Class' ClassName '{' ClassVarDec* subroutineDec* '}'
void compileClass(CompilationEngine *compInfo)
{
    advance(compInfo); // class
    strcpy(compInfo->currentClassName, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // className
    advance(compInfo); // '{'

    // Class variable declarations
    reset(&classTable); // reseting the table before class variable declaration
    while (strcmp(compInfo->tokenizer->currentToken.value, "static") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "field") == 0)
    {
        compileClassVarDec(compInfo);
    }

    // Subroutine declarations
    while (strcmp(compInfo->tokenizer->currentToken.value, "constructor") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "function") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "method") == 0)
    {

        reset(&subroutineTable); // reseting the table before class variable declaration
        compileSubroutine(compInfo);
    }

    advance(compInfo); // '}'
}

// CLASS VARIABLE DECLARATION ----> ( 'static' | 'field' ) type varName (',' varName)* ';'
void compileClassVarDec(CompilationEngine *compInfo)
{
    SymbolKind identKind;
    char identType[100] = "";
    char identName[100] = "";
    SymbolTable st;

    if (strcmp(compInfo->tokenizer->currentToken.value, "static") == 0)
        identKind = SK_STATIC;
    else if (strcmp(compInfo->tokenizer->currentToken.value, "field") == 0)
        identKind = SK_FIELD;
    advance(compInfo); // static | field

    strcpy(identType, compInfo->tokenizer->currentToken.value); // I think this one is also should be "value"
    advance(compInfo);                                  // type

    strcpy(identName, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // varName

    define(identName, identType, identKind);

    // (',' varName)*
    while (strcmp(compInfo->tokenizer->currentToken.value, ",") == 0)
    {
        advance(compInfo);
        strcpy(identName, compInfo->tokenizer->currentToken.value);
        define(identName, identType, identKind);
        advance(compInfo);
    }

    advance(compInfo); // ';'
}

// SUBROUTINE ----> ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody
void compileSubroutine(CompilationEngine *compInfo)
{
    if (strcmp(compInfo->tokenizer->currentToken.value, "method") == 0)
        define("this", compInfo->currentClassName, SK_ARG);

    char subroutineType[100] = "";
    char subroutineReturnType[100] = "";
    char subroutineName[100] = "";
    char functionName[100] = "";

    strcpy(subroutineType, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // constructor | function | method
    strcpy(subroutineReturnType, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // void | type
    strcpy(subroutineName, compInfo->tokenizer->currentToken.value);
    strcpy(compInfo->currentSubroutineName, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // subroutineName
    advance(compInfo); // '('

    compileParameterList(compInfo);

    strcat(functionName, compInfo->currentClassName);
    strcat(functionName, ".");
    strcat(functionName, subroutineName);

    advance(compInfo); // ')'

    // ***** INSTEAD OF USING A SPECIFIC FUNCTION FOR COMPILING STATEMENTS *****
    advance(compInfo); // '{'

    // Compile local variables
    while (strcmp(compInfo->tokenizer->currentToken.value, "var") == 0)
        compileVarDec(compInfo);

    writeFunction(compInfo->vmWriter, functionName, varCount(SK_VAR));

    if (strcmp(subroutineType, "method") == 0)
    {
        writePush(compInfo->vmWriter, segment_to_string(seg_argument), 0); // NEEDS REVIEW
        writePop(compInfo->vmWriter, segment_to_string(seg_pointer), 0);
    }

    else if (strcmp(subroutineType, "constructor") == 0)
    {
        writePush(compInfo->vmWriter, segment_to_string(seg_constant), varCount(SK_FIELD));
        writeCall(compInfo->vmWriter, "Memory.alloc", 1);
        writePop(compInfo->vmWriter, segment_to_string(seg_pointer), 0);
    }

    // Compile statements
    compileStatements(compInfo);

    advance(compInfo); // '}'
}

// PARAMETER LIST ----> ( (type varName) (',' type varName)* )?
void compileParameterList(CompilationEngine *compInfo)
{
    char identName[100] = "";
    char identType[100] = "";

    bool hasParam = false;

    while (strcmp(compInfo->tokenizer->currentToken.value, ")") != 0)
    {
        hasParam = true;
        strcpy(identType, compInfo->tokenizer->currentToken.value);
        advance(compInfo); // type
        strcpy(identName, compInfo->tokenizer->currentToken.value);
        advance(compInfo); // varName

        if (strcmp(compInfo->tokenizer->currentToken.value, ",") == 0)
        {
            define(identName, identType, SK_ARG);
            advance(compInfo);
        }
    }

    if (hasParam)
        define(identName, identType, SK_ARG);
}

// SUBROUTINE BODY ----> '{' varDec* statement '}'
void compileSubroutineBody(CompilationEngine *compInfo)
{
    advance(compInfo); // '{'

    // Compile local variables
    while (strcmp(compInfo->tokenizer->currentToken.value, "var") == 0)
        compileVarDec(compInfo);

    // Compile statements
    compileStatements(compInfo);

    advance(compInfo); // '}'
}

// VARIABLE DECLARATION ----> 'var' type varName (',' varName)* ';'
void compileVarDec(CompilationEngine *compInfo)
{
    char identType[100] = "";
    char identName[100] = "";

    advance(compInfo); // var
    strcpy(identType, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // type
    strcpy(identName, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // varName

    define(identName, identType, SK_VAR);

    // (',' varName)*
    while (strcmp(compInfo->tokenizer->currentToken.value, ",") == 0)
    {
        advance(compInfo);
        strcpy(identName, compInfo->tokenizer->currentToken.value);
        define(identName, identType, SK_VAR);
        advance(compInfo);
    }

    advance(compInfo); // ';'
}

// STATEMENTS ----> statement* : letStatement | ifStatement | whileStatement | doStatement | returnStatement
void compileStatements(CompilationEngine *compInfo)
{
    while (strcmp(compInfo->tokenizer->currentToken.value, "let") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "if") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "while") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "do") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "return") == 0)
    {

        if (strcmp(compInfo->tokenizer->currentToken.value, "let") == 0)
            compileLet(compInfo);

        else if (strcmp(compInfo->tokenizer->currentToken.value, "if") == 0)
            compileIf(compInfo);

        else if (strcmp(compInfo->tokenizer->currentToken.value, "while") == 0)
            compileWhile(compInfo);

        else if (strcmp(compInfo->tokenizer->currentToken.value, "do") == 0)
        {
            compileDo(compInfo);
            writePop(compInfo->vmWriter, "temp", 0);
        }

        else if (strcmp(compInfo->tokenizer->currentToken.value, "return") == 0)
            compileReturn(compInfo);
    }
}

// LET STATEMENT ----> 'let' varName ('[' expression ']')? '=' expression ';'
void compileLet(CompilationEngine *compInfo)
{
    char varName[100] = "";

    advance(compInfo); // let
    strcpy(varName, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // varName

    bool isArray = false;

    if (strcmp(compInfo->tokenizer->currentToken.value, "[") == 0)
    {
        isArray = true;
        advance(compInfo);   // '['
        compileExpression(compInfo); // expression
        advance(compInfo);   // ']'

        writePush(compInfo->vmWriter, kindNames[kindOf(varName)], indexOf(varName));
        writeArithmetic(compInfo->vmWriter, ADD);
    }

    advance(compInfo);   // '='
    compileExpression(compInfo); // expression

    if (strcmp(compInfo->tokenizer->currentToken.value, ";") == 0)
        advance(compInfo); // ';'

    if (isArray)
    {
        // pop into temp value and into pointer to hold for that
        writePop(compInfo->vmWriter, "temp", 0);
        writePop(compInfo->vmWriter, "pointer", 1);
        // put the value into that
        writePush(compInfo->vmWriter, "temp", 0);
        writePop(compInfo->vmWriter, "that", 0);
    }
    else
    {
        // pop directly
        writePop(compInfo->vmWriter, kindNames[kindOf(varName)], indexOf(varName));
    }
}

// IF STATEMENT ----> 'if' '(' expression ')' '{' statement '}' ('else' '{' statement '}')?
void compileIf(CompilationEngine *compInfo)
{
    advance(compInfo);   // if
    advance(compInfo);   // '('
    compileExpression(compInfo); // expression
    advance(compInfo);   // ')'

    char strLabelCounter[10] = "";

    char end_label[50] = "";
    strcat(end_label, compInfo->currentClassName);
    strcat(end_label, "_");
    sprintf(strLabelCounter, "%d", compInfo->labelCounter);
    strcat(end_label, strLabelCounter);

    ++compInfo->labelCounter;

    char else_label[50] = "";
    strcat(else_label, compInfo->currentClassName);
    strcat(else_label, "_");
    sprintf(strLabelCounter, "%d", compInfo->labelCounter);
    strcat(else_label, strLabelCounter);

    ++compInfo->labelCounter;

    writeArithmetic(compInfo->vmWriter, NOT);
    writeIf(compInfo->vmWriter, else_label);

    advance(compInfo);   // '{'
    compileStatements(compInfo); // statements
    advance(compInfo);   // '}'
    
    writeGoto(compInfo->vmWriter, end_label);
    writeLabel(compInfo->vmWriter, else_label);
    
    if (strcmp(compInfo->tokenizer->currentToken.value, "else") == 0)
    {
        advance(compInfo);   // else
        advance(compInfo);   // '{'
        compileStatements(compInfo); // statements
        advance(compInfo);   // '}'
    }

    writeLabel(compInfo->vmWriter, end_label);
}

// WHILE STATEMENT ----> '(' expression ')' '{' statement '}'
void compileWhile(CompilationEngine *compInfo)
{
    char strLabelCounter[10] = "";

    char firstLabel[50] = "";
    strcat(firstLabel, compInfo->currentClassName);
    strcat(firstLabel, "_");
    sprintf(strLabelCounter, "%d", compInfo->labelCounter);
    strcat(firstLabel, strLabelCounter);

    ++compInfo->labelCounter;

    char secondLabel[50] = "";
    strcat(secondLabel, compInfo->currentClassName);
    strcat(secondLabel, "_");
    sprintf(strLabelCounter, "%d", compInfo->labelCounter);
    strcat(secondLabel, strLabelCounter);

    ++compInfo->labelCounter;

    writeLabel(compInfo->vmWriter, firstLabel);

    advance(compInfo);   // while
    advance(compInfo);   // '('
    compileExpression(compInfo); // expression
    advance(compInfo);   // ')'

    writeArithmetic(compInfo->vmWriter, NOT);
    writeIf(compInfo->vmWriter, secondLabel);

    advance(compInfo);   // '{'
    compileStatements(compInfo); // statements
    advance(compInfo);   // '}'

    writeGoto(compInfo->vmWriter, firstLabel);
    writeLabel(compInfo->vmWriter, secondLabel);
}

// DO STATEMENT ----> 'do' subroutineCall ';'
void compileDo(CompilationEngine *compInfo)
{
    char routineName[100] = "";
    int nArgs = 0;

    if (strcmp(compInfo->tokenizer->currentToken.value, "do") == 0)
        advance(compInfo); // do
    strcpy(routineName, compInfo->tokenizer->currentToken.value);
    advance(compInfo); // subroutineName

    if (strcmp(compInfo->tokenizer->currentToken.value, ".") == 0)
    {
        char objectName[100] = "";
        char routineNameP2[100] = "";

        strcpy(objectName, routineName);

        advance(compInfo); // consume '.'

        strcpy(routineNameP2, compInfo->tokenizer->currentToken.value);

        char identType[100] = "";
        if (typeOf(objectName))
            strcpy(identType, typeOf(objectName));

        if (strcmp(identType, "") == 0)
        {
            strcpy(routineName, ""); // OUCH!
            strcat(routineName, objectName);
            strcat(routineName, ".");
            strcat(routineName, routineNameP2);
        }
        else
        {
            ++nArgs;
            writePush(compInfo->vmWriter, kindNames[kindOf(objectName)], indexOf(objectName));

            strcpy(routineName, ""); // OUCH!
            strcat(routineName, typeOf(objectName));
            strcat(routineName, ".");
            strcat(routineName, routineNameP2);
        }

        advance(compInfo); // consume subroutineName Part2 after '.'
        advance(compInfo); // consume '('
        nArgs += compileExpressionList(compInfo);
        advance(compInfo); // consume ')'
        writeCall(compInfo->vmWriter, routineName, nArgs);
        advance(compInfo); // consume ';'
    }

    else if (strcmp(compInfo->tokenizer->currentToken.value, "(") == 0)
    {
        advance(compInfo); // consume '('

        writePush(compInfo->vmWriter, segment_to_string(seg_pointer), 0);

        nArgs = compileExpressionList(compInfo) + 1; // expressionList
        advance(compInfo);                   // ')'
        advance(compInfo);                   // ';'

        char tempRoutineName[100] = "";
        strcat(tempRoutineName, compInfo->currentClassName);
        strcat(tempRoutineName, ".");
        strcat(tempRoutineName, routineName);

        writeCall(compInfo->vmWriter, tempRoutineName, nArgs);
    }
}

// RETURN STATEMENT ----> 'return' expression? ';'
void compileReturn(CompilationEngine *compInfo)
{
    advance(compInfo); // return

    if (strcmp(compInfo->tokenizer->currentToken.value, ";") != 0)
    {
        compileExpression(compInfo); // expression
    }
    else if (strcmp(compInfo->tokenizer->currentToken.value, ";") == 0)
    {
        writePush(compInfo->vmWriter, segment_to_string(seg_constant), 0);
    }

    advance(compInfo); // ';'

    writeReturn(compInfo->vmWriter);
}

// EXPRESSION ----> term (operator term)*
void compileExpression(CompilationEngine *compInfo)
{
    compileTerm(compInfo);

    while (strcmp(compInfo->tokenizer->currentToken.value, "<") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, ">") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "+") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "-") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "*") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "/") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "=") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "|") == 0 ||
           strcmp(compInfo->tokenizer->currentToken.value, "&") == 0)
    {
        if (strcmp(compInfo->tokenizer->currentToken.value, "<") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, LT);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, ">") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, GT);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "+") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, ADD);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "-") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, SUB);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "*") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeCall(compInfo->vmWriter, "Math.multiply", 2);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "/") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeCall(compInfo->vmWriter, "Math.divide", 2);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "=") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, EQ);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "|") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, OR);
        }
        else if (strcmp(compInfo->tokenizer->currentToken.value, "&") == 0)
        {
            advance(compInfo); // op
            compileTerm(compInfo);     // term
            writeArithmetic(compInfo->vmWriter, AND);
        }
    }
}

// TERM ----> integerConstant | stringConstant | keywordConstant | varName |
// varName '[' expression ']' | '(' expression ')' | (unaryOp term) | subroutineCall
void compileTerm(CompilationEngine *compInfo)
{
    // varName | varName '[' expression ']'  |  subroutineCall
    if (compInfo->tokenizer->currentToken.tokType == IDENTIFIER_TOKEN)
    {
        if (*compInfo->tokenizer->cursor == '(' ||
            *compInfo->tokenizer->cursor == '.')
        {
            compileDo(compInfo);
        }
        else if (*compInfo->tokenizer->cursor == '[')
        {
            char identName[100] = "";
            strcpy(identName, compInfo->tokenizer->currentToken.value);
            advance(compInfo); // identifier

            advance(compInfo);   // '['
            compileExpression(compInfo); // expression
            advance(compInfo);   // ']'

            writePush(compInfo->vmWriter, kindNames[kindOf(identName)], indexOf(identName));

            writeArithmetic(compInfo->vmWriter, ADD);
            writePop(compInfo->vmWriter, "pointer", 1);
            writePush(compInfo->vmWriter, "that", 0);
        }
        else
        {
            char identName[100] = "";
            strcpy(identName, compInfo->tokenizer->currentToken.value);
            advance(compInfo); // identifier

            writePush(compInfo->vmWriter, kindNames[kindOf(identName)], indexOf(identName));
        }
    }

    else
    {

        // '(' expression ')'
        if (strcmp(compInfo->tokenizer->currentToken.value, "(") == 0)
        {
            advance(compInfo);   // '('
            compileExpression(compInfo); // expression
            advance(compInfo);   // ')'
        }

        // (unaryOp term)
        else if (strcmp(compInfo->tokenizer->currentToken.value, "~") == 0 ||
                 strcmp(compInfo->tokenizer->currentToken.value, "-") == 0)
        {
            char curSymbol[10] = "";
            strcpy(curSymbol, compInfo->tokenizer->currentToken.value);

            advance(compInfo); // unary operator

            compileTerm(compInfo); // term

            if (strcmp(curSymbol, "-") == 0)
                writeArithmetic(compInfo->vmWriter, NEG);
            else if (strcmp(curSymbol, "~") == 0)
                writeArithmetic(compInfo->vmWriter, NOT);
        }

        // integerConstant
        else if (compInfo->tokenizer->currentToken.tokType == INT_CONST_TOKEN)
        {
            writePush(compInfo->vmWriter, "constant", atoi(compInfo->tokenizer->currentToken.value));
            advance(compInfo); // integer constant
        }

        // stringConstant
        else if (compInfo->tokenizer->currentToken.tokType == STR_CONST_TOKEN)
        {
            char strConst[100] = "";
            strcpy(strConst, compInfo->tokenizer->currentToken.value);
            advance(compInfo); // string constant

            writePush(compInfo->vmWriter, "constant", strlen(strConst));
            writeCall(compInfo->vmWriter, "String.new", 1);

            for (int i = 0; i < strlen(strConst); i++)
            {
                writePush(compInfo->vmWriter, "constant", strConst[i]);
                writeCall(compInfo->vmWriter, "String.appendChar", 2);
            }
        }

        // this - push this pointer
        else if (strcmp(compInfo->tokenizer->currentToken.value, "this") == 0)
        {
            advance(compInfo);
            writePush(compInfo->vmWriter, "pointer", 0);
        }

        // false and null - 0
        else if (strcmp(compInfo->tokenizer->currentToken.value, "null") == 0 ||
                 strcmp(compInfo->tokenizer->currentToken.value, "false") == 0)
        {
            advance(compInfo);
            writePush(compInfo->vmWriter, "constant", 0);
        }

        // true - not 0
        else if (strcmp(compInfo->tokenizer->currentToken.value, "true") == 0)
        {
            advance(compInfo);
            writePush(compInfo->vmWriter, "constant", 1);
            writeArithmetic(compInfo->vmWriter, NEG);
        }
    }
}

// EXPRESSION LIST ----> (expression(',' expression)*)?
int compileExpressionList(CompilationEngine *compInfo)
{
    int nArgs = 0;

    if (strcmp(compInfo->tokenizer->currentToken.value, ")") != 0)
    {
        nArgs = 1;
        compileExpression(compInfo); // expression

        while (strcmp(compInfo->tokenizer->currentToken.value, ",") == 0)
        {
            advance(compInfo);   // ','
            compileExpression(compInfo); // expression
            ++nArgs;                     // another expression
        }
    }

    return nArgs; // number of expressions in the list
}

// COMPILATION INITIALIZATION
void initCompilation(const char *inputFileName)
{
    CompilationEngine compInfo;
    compInfo.tokenizer = initTokenizer(inputFileName);
    compInfo.labelCounter = 0;

    initSymbolTable(&classTable);
    initSymbolTable(&subroutineTable);

    char outputFileName[256] = "";
    char *pch = strstr(inputFileName, ".jack");
    strncpy(outputFileName, inputFileName, strlen(inputFileName) - strlen(pch));
    strcat(outputFileName, ".vm");

    compInfo.vmWriter = vmwriter_init(outputFileName);

    if (!compInfo.tokenizer)
    {
        perror("Error initializing Compilation");
        exit(EXIT_FAILURE);
    }

    startCompilation(&compInfo); // START COMPILATION

    freeTokenizer(compInfo.tokenizer);
}

