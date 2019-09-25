// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// heck.c
// VM to ASM coder

#include "heck.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "utils.h"

// Maximum number of characters for each line in the VM file
#define LINE_MAX 1024

// Maximum number of characters for each token in each instruction
#define TOKEN_MAX 20

// Hash tables used to store heck VM language
table t_heck_push;
table t_heck_pop;
table t_heck_operations;
table t_heck_branching;

// Resultant coded ASM string for a given string
char *parsed_result;

// Counter for ASM jump symbols
int jmp_counter;

void heck_init(void) {
    table_init(&t_heck_push);
    table_init(&t_heck_pop);
    table_init(&t_heck_operations);
    table_init(&t_heck_branching);

    table_push(&t_heck_push, "push", "");
    table_push(&t_heck_push, "constant", "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "local", "@%s\nD=A\n@LCL\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "argument", "@%s\nD=A\n@ARG\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "this", "@%s\nD=A\n@THIS\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "that", "@%s\nD=A\n@THAT\nA=M+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "temp", "@%s\nD=A\n@5\nA=A+D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "static", "@%s.%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");
    table_push(&t_heck_push, "pointer", "@%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1");

    table_push(&t_heck_pop, "pop", "");
    table_push(&t_heck_pop, "constant", "");
    table_push(&t_heck_pop, "local", "@%s\nD=A\n@LCL\nD=D+M\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D");
    table_push(&t_heck_pop, "argument", "@%s\nD=A\n@ARG\nD=D+M\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D");
    table_push(&t_heck_pop, "this", "@%s\nD=A\n@THIS\nD=D+M\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D");
    table_push(&t_heck_pop, "that", "@%s\nD=A\n@THAT\nD=D+M\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D");
    table_push(&t_heck_pop, "temp", "@%s\nD=A\n@5\nD=D+A\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D");
    table_push(&t_heck_pop, "static", "@SP\nM=M-1\nA=M\nD=M\n@%s.%s\nM=D");
    table_push(&t_heck_pop, "pointer", "@SP\nAM=M-1\nD=M\n@%s\nM=D");

    // Operations

    table_push(&t_heck_operations, "add", "@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=D+M\n@SP\nM=M+1");
    table_push(&t_heck_operations, "sub", "@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M-D\n@SP\nM=M+1");

    table_push(&t_heck_operations, "neg", "@SP\nA=M-1\nM=-M");
    table_push(&t_heck_operations, "not", "@SP\nA=M-1\nM=!M");
    table_push(&t_heck_operations, "and", "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D&M");
    table_push(&t_heck_operations, "or", "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nM=D|M");

    table_push(&t_heck_operations, "eq", "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\nM=-1\n@isEq%d\nD;JEQ\n@SP\nA=M-1\nM=0\n(isEq%d)");
    table_push(&t_heck_operations, "lt", "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\nM=-1\n@isLt%d\nD;JLT\n@SP\nA=M-1\nM=0\n(isLt%d)");
    table_push(&t_heck_operations, "gt", "@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\nM=-1\n@isGt%d\nD;JGT\n@SP\nA=M-1\nM=0\n(isGt%d)");

    // Branching

    table_push(&t_heck_branching, "init", "@256\nD=A\n@SP\nM=D");
    table_push(&t_heck_branching, "label", "(%s)");
    table_push(&t_heck_branching, "goto", "@%s\n0;JMP");
    table_push(&t_heck_branching, "if-goto", "@SP\nAM=M-1\nD=M\nM=0\n@%s\nD;JNE");
    table_push(&t_heck_branching, "function", "(%s)");
    table_push(&t_heck_branching, "call",
               "@Return_%s_%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THIS\nD="
               "M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@SP\nD=M\n@%s\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@"
               "LCL\nM=D\n@%s\n0;JMP\n(Return_%s_%d)\n");
    table_push(
        &t_heck_branching, "return",
        "@LCL\nD=M\n@FRAME\nM=D\n@5\nD=D-A\nA=D\nD=M\n@RETURN\nM=D\n@SP\nA=M-1\nD=M\n@ARG\nA=M\nM=D\n@ARG\nD=M\n@SP\nM=D+1\n@FRAME\nAM=M-1\nD=M\n@"
        "THAT\nM=D\n@FRAME\nAM=M-1\nD=M\n@THIS\nM=D\n@FRAME\nAM=M-1\nD=M\n@ARG\nM=D\n@FRAME\nAM=M-1\nD=M\n@LCL\nM=D\n@RETURN\nA=M\n0;JMP\n");

    // Jump counter
    jmp_counter = 0;
}

void heck_flush(void) {
    table_free(&t_heck_push);
    table_free(&t_heck_pop);
    table_free(&t_heck_operations);
    table_free(&t_heck_branching);
    free(parsed_result);
}

// Splits the given string into command, arg1, and arg2.
// Returns 1 if the string is a valid instruction,
// or 0 if otherwise
int heck_parse_line(char *str, char **command, char **arg1, char **arg2) {
    int found = 0;

    *command = malloc(TOKEN_MAX * sizeof(char));
    *arg1 = malloc(TOKEN_MAX * sizeof(char));
    *arg2 = malloc(TOKEN_MAX * sizeof(char));

    return sscanf(str, "%s%s%s", *command, *arg1, *arg2);
}

int heck_is_mem_command(char *str) {
    if (table_value_for(&t_heck_push, str) || table_value_for(&t_heck_pop, str)) {
        return 1;
    }
    return 0;
}

int heck_is_operation(char *str) { return table_value_for(&t_heck_operations, str) != NULL; }

int heck_is_segment(char *str) { return table_value_for(&t_heck_push, str) != NULL; }

int heck_is_branch(char *str) { return table_value_for(&t_heck_branching, str) != NULL; }

table *heck_t_command(char *str) {
    if (strcmp(str, "pop") == 0) {
        return &t_heck_pop;
    }
    if (strcmp(str, "push") == 0) {
        return &t_heck_push;
    }
    return NULL;
}

// Add the necessary code to initiate the program
char *heck_boot(char *filename) {
    // SP = 256
    // Call Sys.init
    return "call Sys.init";
}

char *heck_code(char *str, int line, char *filename) {
    char *cmd = NULL, *seg = NULL, *arg = NULL, *assembler = NULL;

    int tokens = heck_parse_line(str, &cmd, &seg, &arg);

    if (tokens < 1 || tokens > 3) {
        printf("%d\n", tokens);
        printf("%d: line malformed\n", line);
        return 0;
    }

    parsed_result = NULL;

    // Process a command
    // arg1 and arg2 are required
    if (heck_is_mem_command(cmd)) {
        // Check that a segment has been provided
        if (!seg) {
            printf("%d: missing segment for POP/PUSH \n", line);
            return NULL;
        }

        // Check that the segment provided is valid
        if (!heck_is_segment(seg)) {
            printf("%d: invalid segment '%s'\n", line, seg);
            return NULL;
        }

        // Check that an argument has been provided
        if (!arg) {
            printf("%d: missing argument for POP/PUSH \n", line);
            return NULL;
        }

        // Check that the argument is an integer
        char *endptr;
        int n_arg = strtol(arg, &endptr, 10);
        if (endptr == arg) {
            printf("%d: argument '%s' is not an integer\n", line, arg);
            return NULL;
        }

        // Code
        char *s, *tmp;
        size_t len = 0;
        table *t_cmd = heck_t_command(cmd);

        if (strcmp(seg, "pointer") == 0) {
            if (strcmp(arg, "0") == 0) {
                arg = "THIS";
            } else {
                arg = "THAT";
            }
        }

        s = table_value_for(t_cmd, seg);

        len = strlen(s) + 100;
        parsed_result = realloc(parsed_result, len * sizeof(char));
        if (strcmp(seg, "static") == 0) {
            sprintf(parsed_result, s, filename, arg);
        } else {
            sprintf(parsed_result, s, arg);
        }

        return parsed_result;
    }

    // Process an operation
    // No arguments are required
    if (heck_is_operation(cmd)) {
        char *op = table_value_for(&t_heck_operations, cmd);
        size_t len = strlen(op) + 10;
        parsed_result = realloc(parsed_result, len * sizeof(char));

        if ((strcmp(cmd, "eq") == 0) || (strcmp(cmd, "lt") == 0) || (strcmp(cmd, "gt") == 0)) {
            // Generate custom jump labels
            jmp_counter++;
            sprintf(parsed_result, op, jmp_counter, jmp_counter);
            return parsed_result;
        } else {
            strcpy(parsed_result, op);
            return parsed_result;
        }
    }

    // Process a branch operation
    if (heck_is_branch(cmd)) {
        char *op = table_value_for(&t_heck_branching, cmd);

        if (strcmp(cmd, "init") == 0) {
            size_t len = strlen(op) + 1;
            parsed_result = realloc(parsed_result, len * sizeof(char));
            sprintf(parsed_result, op, "");
        }
        if (strcmp(cmd, "label") == 0) {
            size_t len = strlen(op) + strlen(seg) + 1;
            parsed_result = realloc(parsed_result, len * sizeof(char));
            sprintf(parsed_result, op, seg);
        }
        if (strcmp(cmd, "goto") == 0) {
            size_t len = strlen(op) + strlen(seg) + 1;
            parsed_result = realloc(parsed_result, len * sizeof(char));
            sprintf(parsed_result, op, seg);
        }
        if (strcmp(cmd, "function") == 0) {
            // seg = name of the function, xxx.filename
            // arg = number of arguments expected
            size_t len = strlen(op) + strlen(seg) + 1;
            parsed_result = realloc(parsed_result, len * sizeof(char));
            sprintf(parsed_result, op, seg);
        }
        if (strcmp(cmd, "call") == 0) {
            // seg = name of the function, xxx.filename
            // arg = number of arguments passed
            size_t len = strlen(op) + (strlen(seg) * 3) + (sizeof(int) * 3) + 1;
            parsed_result = realloc(parsed_result, len * sizeof(char));
            sprintf(parsed_result, op, seg, jmp_counter, arg, seg, seg, jmp_counter);
            jmp_counter++;
        }
        if (strcmp(cmd, "return") == 0) {
            size_t len = strlen(op) + 1;
            parsed_result = realloc(parsed_result, len * sizeof(char));
            strcpy(parsed_result, op);
        }

        return parsed_result;
    }

    // Free memory
    free(cmd);
    free(seg);
    free(arg);

    // Not found
    printf("%d: unrecognized instruction '%s', len: %zu\n", line, cmd, strlen(cmd));
    return NULL;
}

char *heck_code_end(void) { return "(stop)\n@stop\n0;JMP"; }
