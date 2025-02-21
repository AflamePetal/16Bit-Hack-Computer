#ifndef INCLUDE_SYNANLZ_H
#define INCLUDE_SYNANLZ_H

#include <stdio.h>

#define MAX_TOKEN_LENGTH 256
#define BUFFER_SIZE 1024

// TYPE OF THE CURRENTLY READ TOKEN
typedef enum
{
    KEYWORD_TOKEN,
    SYMBOL_TOKEN,
    IDENTIFIER_TOKEN,
    INT_CONST_TOKEN,
    STR_CONST_TOKEN,
    EOF_TOKEN
} tokenType;

// Token type and it's value
typedef struct
{
    tokenType tokType;
    char value[MAX_TOKEN_LENGTH];
} Token;

// Every variable needed to handle the tokenization
typedef struct
{
    FILE *file;
    char buffer[BUFFER_SIZE];
    char *cursor;
    Token currentToken;
} Tokenizer;

// LIST OF ALL THE KEYWORDS IN THE LANGUAGE
extern const char *keywords[];

#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))

extern const char symbols[];


int compare_keywords(const void *a, const void *b);

Tokenizer *initTokenizer(const char *fileName);

int *isKeyword(const char *str);

int isSymbol(char c);

void clnWhiteSpace(char *buffer);

int getNextToken(Tokenizer *tokenizer);

void freeTokenizer(Tokenizer *tokenizer);

#endif // INCLUDE_SYNANLZ_H