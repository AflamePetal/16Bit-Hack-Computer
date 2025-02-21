#ifndef INCLUDE_VMWRITER_H
#define INCLUDE_VMWRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    FILE *file;
} VMWriter;

// Enum for segments
typedef enum
{
    seg_constant,
    seg_argument,
    seg_local,
    seg_static,
    seg_this,
    seg_that,
    seg_pointer,
    seg_temp
} Segment;

// Enum for arithmetic commands
typedef enum
{
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
} ArithmeticCommand;

VMWriter *vmwriter_init(const char *filename);

const char *segment_to_string(Segment segment);

const char *arithmetic_to_string(ArithmeticCommand command);

void writePush(VMWriter *writer, const char *seg, int index);

void writePop(VMWriter *writer, const char *seg, int index);

void writeArithmetic(VMWriter *writer, ArithmeticCommand command);

void writeLabel(VMWriter *writer, const char *label);

void writeGoto(VMWriter *writer, const char *label);

void writeIf(VMWriter *writer, const char *label);

void writeCall(VMWriter *writer, const char *name, int nArgs);

void writeFunction(VMWriter *writer, const char *name, int nLocals);

void writeReturn(VMWriter *writer);

void vmwriter_close(VMWriter *writer);

#endif // INCLUDE_VMWRITER_H