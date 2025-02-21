/*
    Hash Map data structure is used to store and map the symbols with their opcodes.
    Collisions will be controlled using Open Addressing technique.
*/
#ifndef ASM_TBL_H_INCLUDED
#define ASM_TBL_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TABLE_SIZE 1000

typedef struct
{
    char symbol[50];
    int opcode;
} HashEntity;

typedef struct
{
    HashEntity *table[TABLE_SIZE];
} HashMap;

HashMap symb_map = {{NULL}};
HashMap dest_map = {{NULL}};
HashMap comp_map = {{NULL}};
HashMap jump_map = {{NULL}};

unsigned int hashFunc(const char *symbol)
{
    unsigned int hashVal = 0;
    while (*symbol)
        hashVal = (hashVal << 5) + *symbol++;

    return hashVal % TABLE_SIZE;
}

HashEntity *createEnt(const char *symbol, const int opcode)
{
    HashEntity *newEnt = (HashEntity *)malloc(sizeof(HashEntity));
    strcpy(newEnt->symbol, symbol);
    newEnt->opcode = opcode;
    return newEnt;
}

void addSymbolToTable(HashMap *hMap, const char *symbol, const long opcode)
{
    unsigned int index = hashFunc(symbol);
    // Trying to find an empty chunck in table
    while (hMap->table[index] != NULL && strcmp(hMap->table[index]->symbol, symbol) != 0)
        index = (index + 1) % TABLE_SIZE;

    hMap->table[index] = createEnt(symbol, opcode);
}

int getOpcode(HashMap *hMap, const char *symbol)
{
    unsigned int index = hashFunc(symbol);
    while (hMap->table[index] != NULL)
    {
        if (strcmp(hMap->table[index]->symbol, symbol) == 0)
            return hMap->table[index]->opcode;
        else
            index = (index + 1) % TABLE_SIZE;
    }
    return -1;
}

bool isInTable(HashMap *hMap, const char *symbol)
{
    unsigned int index = hashFunc(symbol);
    unsigned int startIndex = index;
    while (hMap->table[index] != NULL)
    {

        if (strcmp(hMap->table[index]->symbol, symbol) == 0)
            return true;

        index = (index + 1) % TABLE_SIZE;

        if (index == startIndex)
            break;
    }
    return false;
}

static void initSymbTable(HashMap *symbTableMap)
{
    // Numeric symbols: R0, R1, ..., R15
    addSymbolToTable(symbTableMap, "R0", 0x0);
    addSymbolToTable(symbTableMap, "R1", 0x1);
    addSymbolToTable(symbTableMap, "R2", 0x2);
    addSymbolToTable(symbTableMap, "R3", 0x3);
    addSymbolToTable(symbTableMap, "R4", 0x4);
    addSymbolToTable(symbTableMap, "R5", 0x5);
    addSymbolToTable(symbTableMap, "R6", 0x6);
    addSymbolToTable(symbTableMap, "R7", 0x7);
    addSymbolToTable(symbTableMap, "R8", 0x8);
    addSymbolToTable(symbTableMap, "R9", 0x9);
    addSymbolToTable(symbTableMap, "R10", 0xa);
    addSymbolToTable(symbTableMap, "R11", 0xb);
    addSymbolToTable(symbTableMap, "R12", 0xc);
    addSymbolToTable(symbTableMap, "R13", 0xd);
    addSymbolToTable(symbTableMap, "R14", 0xe);
    addSymbolToTable(symbTableMap, "R15", 0xf);

    // Predefined symbols for further developements.
    addSymbolToTable(symbTableMap, "SP", 0x0);
    addSymbolToTable(symbTableMap, "LCL", 0x1);
    addSymbolToTable(symbTableMap, "ARG", 0x2);
    addSymbolToTable(symbTableMap, "THIS", 0x3);
    addSymbolToTable(symbTableMap, "THAT", 0x4);

    // Other predefined symbols
    addSymbolToTable(symbTableMap, "SCREEN", 0x4000);
    addSymbolToTable(symbTableMap, "KBD", 0x6000);

}

static void initDestTable(HashMap *destTableMap)
{
    // DEST symbols
    addSymbolToTable(destTableMap, "null", 0x0);
    addSymbolToTable(destTableMap, "M", 0x1);
    addSymbolToTable(destTableMap, "D", 0x2);
    addSymbolToTable(destTableMap, "MD", 0x3);
    addSymbolToTable(destTableMap, "A", 0x4);
    addSymbolToTable(destTableMap, "AM", 0x5);
    addSymbolToTable(destTableMap, "AD", 0x6);
    addSymbolToTable(destTableMap, "ADM", 0x7);
}

static void initCompTable(HashMap *compTableMap)
{
    // COMP symbols a == 0.
    addSymbolToTable(compTableMap, "0", 0x2a);
    addSymbolToTable(compTableMap, "1", 0x3f);
    addSymbolToTable(compTableMap, "-1", 0x3a);
    addSymbolToTable(compTableMap, "D", 0x0c);
    addSymbolToTable(compTableMap, "A", 0x30);
    addSymbolToTable(compTableMap, "!D", 0x0d);
    addSymbolToTable(compTableMap, "!A", 0x31);
    addSymbolToTable(compTableMap, "-D", 0x0f);
    addSymbolToTable(compTableMap, "-A", 0x33);
    addSymbolToTable(compTableMap, "D+1", 0x1f);
    addSymbolToTable(compTableMap, "A+1", 0x33);
    addSymbolToTable(compTableMap, "D-1", 0x0e);
    addSymbolToTable(compTableMap, "A-1", 0x32);
    addSymbolToTable(compTableMap, "D+A", 0x02);
    addSymbolToTable(compTableMap, "D-A", 0x13);
    addSymbolToTable(compTableMap, "A-D", 0x07);
    addSymbolToTable(compTableMap, "D&A", 0x00);
    addSymbolToTable(compTableMap, "D|A", 0x15);
    // COMP symbols a == 1
    addSymbolToTable(compTableMap, "M", 0x70);
    addSymbolToTable(compTableMap, "!M", 0x71);
    addSymbolToTable(compTableMap, "-M", 0x73);
    addSymbolToTable(compTableMap, "M+1", 0x77);
    addSymbolToTable(compTableMap, "M-1", 0x72);
    addSymbolToTable(compTableMap, "D+M", 0x42);
    addSymbolToTable(compTableMap, "D-M", 0x53);
    addSymbolToTable(compTableMap, "M-D", 0x43);
    addSymbolToTable(compTableMap, "D&M", 0x40);
    addSymbolToTable(compTableMap, "D|M", 0x55);
}

static void initJumpTable(HashMap *jumpTableMap)
{
    // JUMP symbols
    addSymbolToTable(jumpTableMap, "null", 0x0);
    addSymbolToTable(jumpTableMap, "JGT", 0x1);
    addSymbolToTable(jumpTableMap, "JEQ", 0x2);
    addSymbolToTable(jumpTableMap, "JGE", 0x3);
    addSymbolToTable(jumpTableMap, "JLT", 0x4);
    addSymbolToTable(jumpTableMap, "JNE", 0x5);
    addSymbolToTable(jumpTableMap, "JLE", 0x6);
    addSymbolToTable(jumpTableMap, "JMP", 0x7);
}

void initHashMap()
{
    initSymbTable(&symb_map);
    initDestTable(&dest_map);
    initCompTable(&comp_map);
    initJumpTable(&jump_map);
}

#endif // ASM_TBL_H_INCLUDED