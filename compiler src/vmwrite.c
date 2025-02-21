#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmwrite.h"

// Function to create a new VMWriter
VMWriter *vmwriter_init(const char *filename)
{
    VMWriter *writer = (VMWriter *)malloc(sizeof(VMWriter));
    if (!writer)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    writer->file = fopen(filename, "w");
    if (!writer->file)
    {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(1);
    }
    return writer;
}

// // Function to map Segment enum to string
const char *segment_to_string(Segment segment)
{
    static const char *segmentStrings[] = {"constant", "argument", "local", "static", "this", "that", "pointer", "temp"};
    return segmentStrings[segment];
}

// Function to map ArithmeticCommand enum to string
const char *arithmetic_to_string(ArithmeticCommand command)
{
    static const char *commandStrings[] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};
    return commandStrings[command];
}

// Function to write push command
void writePush(VMWriter *writer, const char *seg, int index)
{
    if (strcmp(seg, "var") == 0)
        fprintf(writer->file, "push local %d\n", index);
        
    else if (strcmp(seg, "field") == 0)
        fprintf(writer->file, "push this %d\n", index);

    else
        fprintf(writer->file, "push %s %d\n", seg, index);
}

// Function to write pop command
void writePop(VMWriter *writer, const char *seg, int index)
{
    if (strcmp(seg, "var") == 0)
        fprintf(writer->file, "pop local %d\n", index);

    else if (strcmp(seg, "field") == 0)
        fprintf(writer->file, "pop this %d\n", index);

    else
        fprintf(writer->file, "pop %s %d\n", seg, index);
}

// Function to write arithmetic command
void writeArithmetic(VMWriter *writer, ArithmeticCommand command)
{
    fprintf(writer->file, "%s\n", arithmetic_to_string(command));
}

// Function to write a label
void writeLabel(VMWriter *writer, const char *label)
{
    fprintf(writer->file, "label %s\n", label);
}

// Function to write goto
void writeGoto(VMWriter *writer, const char *label)
{
    fprintf(writer->file, "goto %s\n", label);
}

// Function to write if-goto
void writeIf(VMWriter *writer, const char *label)
{
    fprintf(writer->file, "if-goto %s\n", label);
}

// Function to write function call
void writeCall(VMWriter *writer, const char *name, int nArgs)
{
    fprintf(writer->file, "call %s %d\n", name, nArgs);
}

// Function to write function declaration
void writeFunction(VMWriter *writer, const char *name, int nLocals)
{
    fprintf(writer->file, "function %s %d\n", name, nLocals);
}

// Function to write return statement
void writeReturn(VMWriter *writer)
{
    fprintf(writer->file, "return\n");
}

// Function to close the VMWriter
void vmwriter_close(VMWriter *writer)
{
    if (writer->file)
    {
        fclose(writer->file);
    }
    free(writer);
}