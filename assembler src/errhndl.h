#ifndef ERROR_HANDLER_H_INCLUDED
#define ERROR_HANDLER_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "asmtbl.h"

static FILE *error_log_file = NULL;

void init_error_log()
{
    error_log_file = fopen("Error.log", "w");
    if(!error_log_file)
    {
        fprintf(stderr, "Failed to open the Error log file");
        exit(EXIT_FAILURE);
    }
}

void close_error_log()
{
    if(error_log_file)
        fclose(error_log_file);
}

#define LOG_MESSAGE(msg, lnum) \
    fprintf(stdout, "Message: %s  Line: %ld\n", msg, lnum + 1);

#define LOG_ERROR(msg, lnum) \
    fprintf(error_log_file, "[ERROR] %s  Line: %ld\n", msg, lnum + 1); \
    fflush(error_log_file);

#define HANDLE_ERROR_EXIT(msg, lnum) \
    do                               \
    {                                \
        if(error_log_file){          \
        LOG_ERROR(msg, lnum);        \
        exit(EXIT_FAILURE);          \
               }                     \
    } while (0)

void checkIfValid_comp(const char *symbol, const long lineNum)
{
    if (!isInTable(&comp_map, symbol))
        HANDLE_ERROR_EXIT("Invalid 'COMP' instruction", lineNum);
}

void checkIfValid_dest(const char *symbol, const long lineNum)
{
    if (!isInTable(&dest_map, symbol))
        HANDLE_ERROR_EXIT("Invalid 'DEST' instruction", lineNum);
}

void checkIfValid_jump(const char *symbol, const long lineNum)
{
    if (!isInTable(&jump_map, symbol))
        HANDLE_ERROR_EXIT("Invalid 'JUMP' instruction", lineNum);
}

#endif // ERROR_HANDLER_H_INCLUDED
