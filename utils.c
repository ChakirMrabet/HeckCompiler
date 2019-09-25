// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// utils.h
// Utility library

#include "utils.h"
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int util_is_empty(char *str) { return strlen(str) == 0; }

void util_ltrim(char *str) {
    char *p = str;
    while (*p == ' ') {
        p++;
    }
    if (p != str) {
        memmove(str, p, strlen(p) + 1);
    }
}

void util_rtrim(char *str) {
    char *p = str + strlen(str) - 1;
    while (*p == ' ') {
        p--;
    }
    p++;
    *p = '\0';
}

void util_trim_nl(char *str) {
    char *p = str;
    while (*p != '\0') {
        if (*p == '\n' || *p == '\r' || *p == '\t') {
            *p = '\0';
            return;
        }
        p++;
    }
}

void util_remove_comments(char *str) {
    char *p = strstr(str, "//");
    if (p) {
        *p = '\0';
    }
}

char *util_get_filename(char *filename) {
    char *name = basename(filename);
    char *p = strrchr(name, '.');
    if (p != NULL) {
        *p = '\0';
    }
    return name;
}

char *util_get_file_extension(char *filename) { return (strrchr(filename, '.') + 1); }

char *util_get_dest_filename(char *filename) {
    char *name = util_get_filename(filename);
    char *destFilename = (char *)malloc((strlen(name) + 5) * sizeof(char));
    sprintf(destFilename, "%s.asm", name);
    return destFilename;
}

char *util_get_filename_base(char *filename) {
    char *name = strdup(filename);
    char *p = strrchr(name, '/');

    p++;
    memmove(name, p, strlen(p) + 1);
    return name;
}

int clean_line(char *str) {
    util_remove_comments(str);
    util_ltrim(str);
    util_rtrim(str);
    util_trim_nl(str);
    if (util_is_empty(str)) {
        return 0;
    }
    return 1;
}
