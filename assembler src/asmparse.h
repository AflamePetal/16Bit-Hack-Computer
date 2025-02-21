#ifndef ASM_PARSE_H_INCLUDED
#define ASM_PARSE_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "errhndl.h"
#include "asmtbl.h"

#define MAX_SYMBOL_SIZE 50
#define MAX_DEST_SIZE 10
#define MAX_COMP_SIZE 10
#define MAX_JUMP_SIZE 10

char glob_symb[MAX_SYMBOL_SIZE] = "";
char glob_dest[MAX_DEST_SIZE] = "";
char glob_comp[MAX_COMP_SIZE] = "";
char glob_jump[MAX_JUMP_SIZE] = "";

// Two different passing levels
enum pass_enum
{
    FIRST_PASS,
    SECOND_PASS
} PASS_TIME;

// Type of the current symbol
enum symbolType_enum
{
    nulSymb,
    predSymb,
    digSymb,
    varSymb
} SYMBOL_TYPE;

// Type of the current instruction
enum INST_TYPE_enum
{
    NULL_INSTRUCTION,
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION
} CUR_INST_TYPE;

static long lineNum = 0; // Stores the line number when reading and processing lines

static int onTopOf = 0x10; // Since we've stored R0 to R15 symbols to our table and
                           // We have mapped to them numbers from 0 to 15, if we want to add new
                           // symbols to the table, we should add 'on top of' them. starting from number 16.

// CONTROLLING 'L' INSTRUCTIONS
static void L_inst_ctrl(char *line)
{
    // Removing leading white-space characters
    const char *ptr = line;
    while (isspace((unsigned char)*ptr))
        ptr++;
    memmove(line, ptr, strlen(ptr) + 1); // It's +1 to add the null terminator character

    if (line[0] == '@')
        ++lineNum;
    else if (strpbrk(line, "=;") != NULL && strstr(line, "//") == NULL)
        ++lineNum;

    else if (line[0] == '(' && line[strlen(line) - 2] == ')')
    {
        char curSymb[MAX_SYMBOL_SIZE];
        strncpy(curSymb, line + 1, strlen(line) - 2);
        curSymb[strlen(line) - 3] = '\0'; // Add null terminator

        if (!isInTable(&symb_map, curSymb))
            addSymbolToTable(&symb_map, curSymb, lineNum);

        strcpy(glob_symb, curSymb);
        CUR_INST_TYPE = L_INSTRUCTION;
    }
}

// CONTROLLING 'A' INSTRUCTIONS
/*
    The trick in this function is that some strings that come after the
    symbol @, are not necessarily symbols but can be numbers. So if the function
    ended up considering them as numbers, we won't return their opcodes for 
    further operations, but converting them to integer form otherwise.
*/
static void A_inst_ctrl(char *line)
{
    char curSym[50];
    strcpy(curSym, line + 1);
    curSym[strlen(curSym) - 1] = '\0';
    SYMBOL_TYPE = digSymb;

    if (isInTable(&symb_map, curSym))
    {
        strcpy(glob_symb, curSym);
        SYMBOL_TYPE = varSymb;
    }

    // Adding new symbols that are not in a form "(symbol)".
    else
    {
        char *pch = curSym;
        while (*pch != '\0')
        {
            if (isalpha(*pch))
            {
                addSymbolToTable(&symb_map, curSym, onTopOf++);
                SYMBOL_TYPE = varSymb;
                break;
            }
            ++pch;
        }
        strcpy(glob_symb, curSym);
    }

    CUR_INST_TYPE = A_INSTRUCTION;
}

// CONTROLLING 'C' INSTRUCTIONS
static void C_inst_ctrl(char *line)
{
    // If the instruction contains '=' it's in the form of 'DEST = COMP'.
    if(strchr(line, '='))
    {
        char *parts[2];

        parts[0] = strtok(line, " \n=");
        parts[1] = strtok(NULL, " \n=");

        strcpy(glob_dest, parts[0]);
        strcpy(glob_comp, parts[1]);
    }

    // If the instruction contains ';' it's in the form of 'COMP ; JUMP'.
    else if(strchr(line, ';'))
    {
        char *parts[2];

        parts[0] = strtok(line, " \n;");
        parts[1] = strtok(NULL, " \n;");

        strcpy(glob_comp, parts[0]);
        strcpy(glob_jump, parts[1]);
    }

    CUR_INST_TYPE = C_INSTRUCTION;
}

// Process line and choose a right intruction
static int lineProcess(char *line)
{
    // Removing leading white-space characters
    const char *ptr = line;
    while (isspace((unsigned char)*ptr))
        ptr++;
    memmove(line, ptr, strlen(ptr) + 1); // It's +1 to add the null terminator character

    /***** A_INSTRUCTION ******/
    if (line[0] == '@')
    {
        A_inst_ctrl(line);

        ++lineNum; // Increasing the line number
    }

    /***** C_INSTRUCTION ******/
    else if (line[0] != '/' && line[1] != '/')
    {
        if (strpbrk(line, "=;") != NULL)
        {
            C_inst_ctrl(line);

            ++lineNum; // Increasing the line number
        }
        else
            CUR_INST_TYPE = NULL_INSTRUCTION;
    }
    else
        CUR_INST_TYPE = NULL_INSTRUCTION;
}

/*
    passing two times through the whole lines of the file, but each
    time for a different reason. One time to add new symbols to the table
    and the second time for making sense of the codes.
*/
int parseLine(FILE *myFile)
{
    char line[256];

    if (fgets(line, 256, myFile) != NULL)
    {
        // Freeing up the character buffers to prevent from possible bugs
        strcpy(glob_symb, "");
        strcpy(glob_dest, "");
        strcpy(glob_comp, "");
        strcpy(glob_jump, "");

        if (PASS_TIME == FIRST_PASS)
            L_inst_ctrl(line);

        else if (PASS_TIME == SECOND_PASS)
            lineProcess(line);

        return 1;
    }
    else
    {
        LOG_MESSAGE("No more lines to read", lineNum);
        lineNum = 0;
        return 0;
    }
}



#endif // ASM_PARSE_H_INCLUDED