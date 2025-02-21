#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "synanlz.h"

const char *keywords[] = {
    "class", "constructor", "function", "method", "field", "static", "var",
    "int", "char", "boolean", "void", "true", "false", "null", "this",
    "let", "do", "if", "else", "while", "return"};

const char symbols[] = "{}()[].,;+-*/&|<>=~";

int compare_keywords(const void *a, const void *b)
{
    const char * const *pa = a;
    const char * const *pb = b;
    return strcmp(*pa, *pb);
}

// Initializes the tokenizer struct
Tokenizer *initTokenizer(const char *fileName)
{
    Tokenizer *tokenizer = (Tokenizer *)malloc(sizeof(Tokenizer));
    tokenizer->file = fopen(fileName, "r");
    if (!tokenizer->file)
    {
        perror("Error opening file");
        free(tokenizer);
        return NULL;
    }
    tokenizer->buffer[0] = '\0';
    tokenizer->cursor = tokenizer->buffer;

    // Sorting the keywords' elements for furthure binary search
    qsort(keywords, NUM_KEYWORDS, sizeof(char *), compare_keywords);

    return tokenizer;
}

// Checks whether 'str' is a keyword or not
// Simple, linear search because the number of elements is not significant
int *isKeyword(const char *str)
{
    return bsearch(&str, keywords, NUM_KEYWORDS, sizeof(char *), compare_keywords);
}

// Checks if 'c' is one of the symbols in 'symbols' string
int isSymbol(char c)
{
    return strchr(symbols, c) != NULL;
}

// Cleans the leading whitespace characters
void clnWhiteSpace(char *buffer)
{
    char *pch = strdup(buffer);
    while (*pch == ' ' || *pch == '\t' || *pch == '\v')
        pch++;
    memmove(buffer, pch, strlen(pch) + 1);
}

// Reads the new token if there is any
int getNextToken(Tokenizer *tokenizer)
{
    if (!tokenizer->file)
        return 0;

    char *cursor = tokenizer->cursor;
    char token[MAX_TOKEN_LENGTH];
    int tokenIndex = 0;

    // Ignore whitespaces and null characters
    while (*cursor == '\0' || *cursor == '\n')
    {
        if (!fgets(tokenizer->buffer, BUFFER_SIZE, tokenizer->file)) // If not able to read from the file, then it's the end of the file
        {
            tokenizer->currentToken.tokType = EOF_TOKEN;
            strcpy(tokenizer->currentToken.value, "EOF");
            return 0;
        }
        cursor = tokenizer->buffer;
    }

    clnWhiteSpace(cursor);

    // Ignore comments and new-line characters
    while ((*cursor == '/' && (*(cursor + 1) == '/' || *(cursor + 1) == '*')) || *cursor == '\n')
    {
        if (*cursor == '\n')
        {
            fgets(tokenizer->buffer, BUFFER_SIZE, tokenizer->file);
            cursor = tokenizer->buffer;
            clnWhiteSpace(cursor);
        }

        else if (*(cursor + 1) == '/')
        {
            fgets(tokenizer->buffer, BUFFER_SIZE, tokenizer->file);
            cursor = tokenizer->buffer;
            clnWhiteSpace(cursor);
        }
        else // comments of the form /*...*/
        {
            cursor += 2;
            while (*cursor && !(*cursor == '*' && *(cursor + 1) == '/')) // Searching for end of the comment block
            {
                cursor++;
                if (*cursor == '\0') // If '*/' is not in the current buffer, try to read new string of characters until it's found
                {
                    fgets(tokenizer->buffer, BUFFER_SIZE, tokenizer->file);
                    cursor = tokenizer->buffer;
                    clnWhiteSpace(cursor);
                }
            }
            cursor += 2;
            clnWhiteSpace(cursor);
        }
    }

    // IDENTIFIER | KEYWORD 
    if (isalpha(*cursor) || *cursor == '_') // First character should be alphabetical or '_'
    {
        while (isalnum(*cursor) || *cursor == '_') // Other characters could be one of these
            token[tokenIndex++] = *cursor++;

        token[tokenIndex++] = '\0';
        tokenizer->currentToken.tokType = isKeyword(token) ? KEYWORD_TOKEN : IDENTIFIER_TOKEN; // Check if is keyword
    }

    // INTEGER CONSTANT
    else if (isdigit(*cursor))
    {
        while (isdigit(*cursor))
            token[tokenIndex++] = *cursor++;

        token[tokenIndex] = '\0';
        tokenizer->currentToken.tokType = INT_CONST_TOKEN;
    }

    // STRING CONSTANT
    else if (*cursor == '"')
    {
        ++cursor;
        while (*cursor != '"')
            token[tokenIndex++] = *cursor++;

        ++cursor;
        token[tokenIndex] = '\0';
        tokenizer->currentToken.tokType = STR_CONST_TOKEN;
    }

    // SYMBOL
    else if (isSymbol(*cursor) && *cursor != '\0')
    {
        token[0] = *cursor++;
        token[1] = '\0';
        tokenizer->currentToken.tokType = SYMBOL_TOKEN;
    }

    // Any other and unexpected characters
    else
    {
        fprintf(stderr, "File:%s\tError: Unexpected character '%c'\n", tokenizer->file ,*cursor);
        return 0;
    }

    strcpy(tokenizer->currentToken.value, token);
    tokenizer->cursor = cursor;
    return 1;
}

// MEMORY DEALLOCATION
void freeTokenizer(Tokenizer *tokenizer)
{
    if (tokenizer->file)
        fclose(tokenizer->file);
    free(tokenizer);
}