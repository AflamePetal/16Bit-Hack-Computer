#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "asmtbl.h"
#include "asmparse.h"
#include "errhndl.h"

#define INPUT_FILE "D:\\Personal Data\\Programming Projects\\C\\Hack_Assembler\\Test_Asm_Files\\Rect.asm"
#define OUTPUT_FILE "Machine_Code.txt"

/*
    These macros are supposed to provide a setting to print out
    binary numbers.
*/
#define _8_BIT_PATTERN "%c%c%c%c%c%c%c%c"
#define INT_TO_BINARY(byte)          \
        ((byte) & 0x80 ? '1' : '0'), \
        ((byte) & 0x40 ? '1' : '0'), \
        ((byte) & 0x20 ? '1' : '0'), \
        ((byte) & 0x10 ? '1' : '0'), \
        ((byte) & 0x08 ? '1' : '0'), \
        ((byte) & 0x04 ? '1' : '0'), \
        ((byte) & 0x02 ? '1' : '0'), \
        ((byte) & 0x01 ? '1' : '0')

#define DEST_PATTERN "%c%c%c"
#define INT_TO_DEST_PATTERN(inst)    \
        ((inst) & 0x04 ? '1' : '0'), \
        ((inst) & 0x02 ? '1' : '0'), \
        ((inst) & 0x01 ? '1' : '0')

#define JUMP_PATTERN "%c%c%c"
#define INT_TO_JUMP_PATTERN(inst)    \
        ((inst) & 0x04 ? '1' : '0'), \
        ((inst) & 0x02 ? '1' : '0'), \
        ((inst) & 0x01 ? '1' : '0')

#define COMP_PATTERN "%c%c%c%c%c%c"
#define INT_TO_COMP_PATTERN(inst)    \
        ((inst) & 0x20 ? '1' : '0'), \
        ((inst) & 0x10 ? '1' : '0'), \
        ((inst) & 0x08 ? '1' : '0'), \
        ((inst) & 0x04 ? '1' : '0'), \
        ((inst) & 0x02 ? '1' : '0'), \
        ((inst) & 0x01 ? '1' : '0')

// Print out the A type instruction
void A_Inst_binOutput(FILE *outFile, int intCode)
{
    fprintf(outFile, ""_8_BIT_PATTERN""_8_BIT_PATTERN"\n",
            INT_TO_BINARY(intCode >> 8), INT_TO_BINARY(intCode));
}

// Print out the C type instruction
void C_Inst_binOutput(FILE *outFile, char a_bit, int destCode, int compCode, int jumpCode)
{
    fprintf(outFile, "111%c" COMP_PATTERN "" DEST_PATTERN "" JUMP_PATTERN "\n",
            a_bit, INT_TO_COMP_PATTERN(compCode), INT_TO_DEST_PATTERN(destCode), INT_TO_JUMP_PATTERN(jumpCode));
}

void assembleMain(FILE *asmFile, FILE *outFile)
{
    initHashMap(); // Initializing our hash map with all the predefined symbols

    PASS_TIME = FIRST_PASS;
    while (parseLine(asmFile) == 1)
    {
        // Goes through the lines of the file just because PASS_TIME == FIRST_PASS
        // It keeps track of (xxx) L type instructions to add them to the table.
    }

    rewind(asmFile); // Turning the file pointer back to the beginning, for another pass

    PASS_TIME = SECOND_PASS;
    while (parseLine(asmFile) == 1)
    {
        if (CUR_INST_TYPE == A_INSTRUCTION)
        {
            if (SYMBOL_TYPE == digSymb)
            {
                int digitVar = atoi(glob_symb);
                A_Inst_binOutput(outFile, digitVar);
            }
            else
            {
                int opCode = getOpcode(&symb_map, glob_symb);
                A_Inst_binOutput(outFile, opCode);
            }
        }
        else if (CUR_INST_TYPE == C_INSTRUCTION)
        {
            int destCode = getOpcode(&dest_map, glob_dest);
            if (destCode == -1)
                destCode = 0;

            int compCode = getOpcode(&comp_map, glob_comp);
            if (compCode == -1)
                compCode = 0;

            int jumpCode = getOpcode(&jump_map, glob_jump);
            if (jumpCode == -1)
                jumpCode = 0;
            
            char a_bit = '0';
            if(strchr(glob_comp, 'M'))
                a_bit = '1';
            C_Inst_binOutput(outFile, a_bit, destCode, compCode, jumpCode);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <source .asm file>\n", argv[0]);
        return 1;
    }

    init_error_log(); // Initializing an error log file for demonstrating errors

    FILE *asmFile;
    asmFile = fopen(argv[1], "r");
    if (!asmFile)
    {
        printf("\nError opening the .asm file\n");
        getchar();
        exit(EXIT_FAILURE);
    }

    FILE *outFile;
    outFile = fopen(""OUTPUT_FILE"", "w");
    if (!outFile)
    {
        printf("\nError opening the output file\n");
        getchar();
        exit(EXIT_FAILURE);
    }

    assembleMain(asmFile, outFile);

    fclose(asmFile);
    fclose(outFile);

    close_error_log(); // Closing the error log file
    return 0;
}