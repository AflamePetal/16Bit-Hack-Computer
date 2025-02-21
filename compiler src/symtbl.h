#ifndef INCLUDE_SYMTBL_H
#define INCLUDE_SYMTBL_H

#define TABLE_SIZE 211

typedef enum
{
    SK_STATIC,
    SK_FIELD,
    SK_ARG,
    SK_VAR,
    SK_NONE
} SymbolKind;

typedef struct Symbol
{
    char name[50];
    char type[50];
    int kindIndex;
    int globIndex; // Not part of the compiler structer, but it's there to help with reseting!
    SymbolKind kind;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable
{
    Symbol *symbols[TABLE_SIZE];
    Symbol *headOfTable; // store the address of the latest symbol that is added to the table for further operations.
    int kind_count[4];   // Count number of variables of each kind(STATIC, FIELD, ARG, VAR, NONE) available in the table
    int symbCount;
} SymbolTable;


// Global symbol tables
// classTable: holds class-scope symbols (static and field)
// subTable: holds subroutine-scope symbols (argument and var)
SymbolTable classTable;
SymbolTable subroutineTable;


extern const char *kindNames[];


void initSymbolTable(SymbolTable *table);

unsigned int hash(const char *key);

Symbol *lookupInTable(SymbolTable *table, const char *name);

Symbol *lookupSymbol(const char *name);

void reset(SymbolTable *table);

void insertSymbol(SymbolTable *table, const char *name, const char *type, SymbolKind kind);

void define(const char *name, const char *type, SymbolKind kind);

int varCount(SymbolKind kind);

SymbolKind kindOf(const char *name);

const char *typeOf(const char *name);

int indexOf(const char *name);

#endif // INCLUDE_SYMTBL_H
