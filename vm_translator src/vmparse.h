#ifndef INCLUDE_VM_PARSE_H
#define INCLUDE_VM_PARSE_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "vmcodewrite.h"
#include "vmtrack.h"

static int lineNum = 0; // stores numbers of lines read

/*
    Function "handleCommand", decides wich function to be called based on the
    instructions' tokens.
*/
void handleCommand(FILE *outputFile, char *tok1, char *tok2, char *tok3)
{
    // Stack commands
    if (strcmp(tok1, "push") == 0)
    {
        if (strcmp(tok2, "constant") == 0)
            pushConst(outputFile, tok3);
        else if (strcmp(tok2, "argument") == 0)
            pushArg(outputFile, tok3);
        else if (strcmp(tok2, "local") == 0)
            pushLcl(outputFile, tok3);
        else if (strcmp(tok2, "pointer") == 0)
            pushPtr(outputFile, tok3);
        else if (strcmp(tok2, "static") == 0)
            pushStat(outputFile, tok3);
        else if (strcmp(tok2, "this") == 0)
            pushThis(outputFile, tok3);
        else if (strcmp(tok2, "that") == 0)
            pushThat(outputFile, tok3);
        else if (strcmp(tok2, "temp") == 0)
            pushTemp(outputFile, tok3);
    }
    else if (strcmp(tok1, "pop") == 0)
    {
        if (strcmp(tok2, "argument") == 0)
            popArg(outputFile, tok3);
        else if (strcmp(tok2, "local") == 0)
            popLcl(outputFile, tok3);
        else if (strcmp(tok2, "pointer") == 0)
            popPtr(outputFile, tok3);
        else if (strcmp(tok2, "static") == 0)
            popStat(outputFile, tok3);
        else if (strcmp(tok2, "this") == 0)
            popThis(outputFile, tok3);
        else if (strcmp(tok2, "that") == 0)
            popThat(outputFile, tok3);
        else if (strcmp(tok2, "temp") == 0)
            popTemp(outputFile, tok3);
    }

    // Arithmetic commands
    else if (strcmp(tok1, "add") == 0)
        addOp(outputFile);
    else if (strcmp(tok1, "sub") == 0)
        subOp(outputFile);
    else if (strcmp(tok1, "neg") == 0)
        negOp(outputFile);

    // Logic commands
    else if (strcmp(tok1, "and") == 0)
        andOp(outputFile);
    else if (strcmp(tok1, "or") == 0)
        orOp(outputFile);
    else if (strcmp(tok1, "not") == 0)
        notOp(outputFile);

    // Conditional commands
    else if (strcmp(tok1, "eq") == 0)
    {
        ++eqCount;
        eqOp(outputFile);
    }
    else if (strcmp(tok1, "gt") == 0)
    {
        ++gtCount;
        gtOp(outputFile);
    }
    else if (strcmp(tok1, "lt") == 0)
    {
        ++ltCount;
        ltOp(outputFile);
    }

    // Branching commands
    else if (strcmp(tok1, "goto") == 0)
        gotoCommand(outputFile, tok2);
    else if (strcmp(tok1, "if-goto") == 0)
        ifgotoCommand(outputFile, tok2);
    else if (strcmp(tok1, "label") == 0)
        labelCommand(outputFile, tok2);

    // Function commands
    else if (strcmp(tok1, "call") == 0)
    {
        ++retCount;
        callCommand(outputFile, tok2, tok3);
    }
    else if (strcmp(tok1, "function") == 0)
    {
        pushFunc(tok1);
        functionCommand(outputFile, tok2, tok3);
    }
    else if (strcmp(tok1, "return") == 0)
    {
        popFunc();
        returnCommand(outputFile);
    }
}

// Tokenize the current line
char **lineTok(const char *line)
{
    char *copy_line = strdup(line);

    // Allocate memmory for storing tokens
    char **parts = (char **)malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++)
        parts[i] = (char *)malloc(50 * sizeof(char));

    // tokenizing the instruction
    parts[0] = strtok(copy_line, " \n\0");
    for (int i = 1; i < 3; i++)
        parts[i] = strtok(NULL, " \n\0");

    return parts;
}

// It cleans the connected whitespaces and comments in a line.
void clnLine(char *line)
{
    // Removing leading white-space characters
    char *pch = line;
    while (isspace((unsigned char)*pch))
        pch++;
    memmove(line, pch, strlen(pch) + 1); // It's +1 to add the null terminator character

    // Ignore comments available at front of the instructions
    char *commentPart = strstr(pch, "//");
    if (commentPart)
        *commentPart = '\0';
}

// Make sense of virtual machine code
int parseLine(FILE *inputFile, FILE *outputFile)
{
    char line[256];

    // Read lines
    while (fgets(line, 256, inputFile) != NULL)
    {
        // Ignoring leading whitespaces and forwarding comments
        clnLine(line);

        if (line[0])
        {
            // Tokenize
            char **parts = lineTok(line);

            // Handling commands and writing out the asm codes
            handleCommand(outputFile, parts[0], parts[1], parts[2]);

            // Freeing up the memmory allocated back in "lineTok" function
            for (int i = 0; i < 3; i++)
            {
                free(parts[i]);
            }
            free(parts);

            ++lineNum;
        }
    }

    // End of the file notification
    fprintf(stderr, "Line %d: End of the file\n", lineNum);
    return 1;
}

#endif // INCLUDE_VM_PARSE_H