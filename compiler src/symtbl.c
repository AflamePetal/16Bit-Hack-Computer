#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtbl.h"

const char *kindNames[] = {"static", "field", "argument", "var", "none"};

// Initialization
void initSymbolTable(SymbolTable *table)
{
    for (int i = 0; i < TABLE_SIZE; i++)
        table->symbols[i] = NULL;

    for (int i = 0; i < 4; i++)
        table->kind_count[i] = 0;

    table->symbCount = 0;
}

// Hash function
unsigned int hash(const char *key)
{
    char *keyName = strdup(key);
    unsigned long h = 5381;
    int c;

    while ((c = *keyName++))
        h = ((h << 5) + h) + c; /* h * 33 + c */

    return h % TABLE_SIZE;
}

// Returns a pointer to the Symbol if found; otherwise, returns NULL.
Symbol *lookupInTable(SymbolTable *table, const char *name)
{
    unsigned int index = hash(name);
    unsigned int startIndex = index;

    while (table->symbols[index] != NULL)
    {
        if (strcmp(table->symbols[index]->name, name) == 0 && table->symbols[index])
            return table->symbols[index];

        index = (index + 1) % TABLE_SIZE;
        if (index == startIndex) // We've probed all slots
            break;
    }

    return NULL;
}

// Lookup a symbol (first checking the subroutine table, then the class table).
Symbol *lookupSymbol(const char *name)
{
    Symbol *sym = lookupInTable(&subroutineTable, name);
    if (sym)
        return sym;
    else
        return lookupInTable(&classTable, name);
}

// Free all symbols in the given table (used for subroutine table cleanup).
// POOR ALGORITHM !!!
void reset(SymbolTable *table)
{
    Symbol *curSymbol = table->headOfTable;
    while (curSymbol != NULL)
    {
        Symbol *temp = curSymbol;
        if (curSymbol->globIndex != 0)
            curSymbol = curSymbol->next;
        else
            curSymbol = NULL;

        free(temp);
    }

    for (int i = 0; i < TABLE_SIZE; i++)
        table->symbols[i] = NULL;

    for (int i = 0; i < 4; i++)
        table->kind_count[i] = 0;

    table->symbCount = 0;
}

// Insert a new symbol into the given table.
void insertSymbol(SymbolTable *table, const char *name, const char *type, SymbolKind kind)
{
    // Check if the symbol is already in the table or not
    if (lookupInTable(table, name))
    {
        printf("Symbol is already in the subroutine table.\nCan not be added.\n");
        exit(EXIT_FAILURE);
    }

    unsigned int h = hash(name);
    // Try to find an empty chunk. If not found report and exit.
    int count = 0;
    while (table->symbols[h] != NULL)
    {
        h = (h + 1) % TABLE_SIZE;
        if (count == TABLE_SIZE)
        {
            printf("Symbol Table is full\n");
            exit(EXIT_FAILURE);
        }
        ++count;
    }

    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    if (!sym)
    {
        perror("Allocation Error for Symbol struct\n");
        exit(EXIT_FAILURE);
    }

    strcpy(sym->name, name);
    strcpy(sym->type, type);
    sym->kind = kind;

    // Use counts array to assign index (only for the four kinds we track)
    int countIndex = (int)kind; // valid for SK_STATIC, SK_FIELD, SK_ARG, SK_VAR
    sym->kindIndex = table->kind_count[countIndex];
    table->kind_count[countIndex]++;

    sym->globIndex = table->symbCount++;

    // Insert at beginning of the symbols list.
    sym->next = table->headOfTable;
    table->symbols[h] = sym;
    table->headOfTable = sym;
}

// API: define – adds a new identifier to the appropriate symbol table.
void define(const char *name, const char *type, SymbolKind kind)
{
    // For class-scope symbols, use classTable; for subroutine-scope, use subTable.
    if (kind == SK_STATIC || kind == SK_FIELD)
        insertSymbol(&classTable, name, type, kind);

    else if (kind == SK_ARG || kind == SK_VAR)
        insertSymbol(&subroutineTable, name, type, kind);
}

// API: varCount – returns the number of variables of the given kind in the current scope.
int varCount(SymbolKind kind)
{
    if (kind == SK_STATIC || kind == SK_FIELD)
        return classTable.kind_count[(int)kind];

    else if (kind == SK_ARG || kind == SK_VAR)
        return subroutineTable.kind_count[(int)kind];

    return 0;
}

// API: kindOf – returns the kind of the named identifier.
SymbolKind kindOf(const char *name)
{
    Symbol *sym = lookupSymbol(name);
    return sym ? sym->kind : SK_NONE;
}

// API: typeOf – returns the type of the named identifier.
const char *typeOf(const char *name)
{
    Symbol *sym = lookupSymbol(name);
    return sym ? sym->type : NULL;
}

// API: indexOf – returns the index assigned to the named identifier.
int indexOf(const char *name)
{
    Symbol *sym = lookupSymbol(name);
    return sym ? sym->kindIndex : -1;
}