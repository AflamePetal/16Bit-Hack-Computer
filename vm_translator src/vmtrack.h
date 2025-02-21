/*
    This file is intended to provide additional functions which all
    will resolve and control the problem of translating multiple
    files into a single .asm file.
    Functions will notify: 
    1.Current .vm file that is being read
    2.Currently called function
*/

#ifndef INCLUDE_TRACKER_H
#define INCLUDE_TRACKER_H

#include <stdio.h>
#include <string.h>

static char curFunc[1000][50]; // array that will work as stack to store the name of currently read function
static int idx = 0; // index of the array

static char curFileName[50];

// FUNCTIONS
void pushFunc(const char *funcName)
{
    ++idx;
    strcpy(curFunc[idx], funcName);
}

void popFunc()
{
    --idx;
}

char *getCurFuncName()
{
    return curFunc[idx];
}

// FILES
char *setFileName(const char *fileName)
{
    strcpy(curFileName, fileName);
}

char *getCurFileName()
{
    char *formatPart = strstr(curFileName, ".vm");
    if (formatPart)
        *formatPart = '\0';
    return curFileName;
}


#endif // INCLUDE_TRACKER_H