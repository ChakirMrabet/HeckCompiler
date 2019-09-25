// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// main.c

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "heck.h"
#include "utils.h"

// Maximum number of characters for a source line
#define LINE_MAX 1024

// Shows welcome screen and parses arguments used to call the
// program. If none, or insuficient, shows help and returns 0.
// If successful, returns the number of files provided, and
// their names in file_names array

int prompt_screen(int args, char *argv[], int *add_comments, char ***file_names) {
    puts("VMTranslator for HACK Machines, v1.0");
    puts("Chakir Mrabet, Nand2Tetris, Part2, 2019\n");

    // Check if the user has provided the source filename
    if (args < 2) {
        puts("ERROR: VM filename or directory is missing!\n");
        puts("Use as follows:\n");
        puts("\tVMTranslator <filename> [-c]\n");
        puts("\tVMTranslator <directory> [-c]\n");
        puts("-c: add comments to generated ASM file\n");
        return 0;
    }

    // Check the flags provided by the user
    if (args > 2 && strcmp(argv[2], "-c") == 0) {
        *add_comments = 1;
    }

    // Check what has been provided
    char *file_name = argv[1];
    struct stat file_info;

    if (stat(file_name, &file_info) == 0) {
        // A file
        if (S_ISREG(file_info.st_mode)) {
            if (strcmp("vm", util_get_file_extension(file_name)) == 0) {
                *file_names = (char **)malloc(sizeof(char *));
                **file_names = (char *)malloc(strlen(file_name) * sizeof(char));
                strcpy(**file_names, file_name);
                return 1;
            } else {
                printf("ERROR: File '%s' is not of VM type.\n", file_name);
                return 0;
            }
        }
        // A directory
        if (S_ISDIR(file_info.st_mode)) {
            // List all files with .VM extension
            // allocate memory in the array to hold all the names

            struct dirent *ep;
            DIR *dp;
            int count = 0;
            size_t str_len = 0;
            char **tmp = NULL;

            dp = opendir(file_name);

            if (dp != NULL) {
                while ((ep = readdir(dp))) {
                    if (strcmp("vm", util_get_file_extension(ep->d_name)) == 0) {
                        str_len = strlen(file_name) + strlen(ep->d_name) + 1;

                        tmp = (char **)realloc(*file_names, (count + 1) * sizeof(char *));
                        if (tmp != NULL) {
                            *file_names = tmp;
                            (*file_names)[count] = (char *)malloc(str_len * sizeof(char));
                            sprintf((*file_names)[count], "%s%s", file_name, ep->d_name);
                            count++;
                        } else {
                            puts("ERROR: Could not allocate memory for the list of files");
                        }
                    }
                }
                closedir(dp);
            }

            return count;
        }
    }

    // Something went wrong
    return 0;
}

// Writes the given string to the given file pointer
void write_file(FILE *fp, char *asmcode, char *vm, int add_comments) {
    if (add_comments && vm != NULL) {
        fprintf(fp, "\n// %s\n\n", vm);
    }
    if (asmcode && asmcode != NULL) {
        fprintf(fp, "%s\n", asmcode);
    }
}

int main(int args, char *argv[]) {
    // When enabled, each ASM parsed instruction will have a comment
    // with the original VM instruction
    int addComments = 0;

    // Contains the names of the files to iterate through
    char **files = NULL;

    // Contains the number of files to iterate through
    int n_files = prompt_screen(args, argv, &addComments, &files);

    // If no files found, exit
    if (n_files < 1) {
        return 0;
    }

    // Create and open destiniation file
    char *destFilename = util_get_dest_filename(argv[1]);
    FILE *fpdest = fopen(destFilename, "wa");

    if (!fpdest) {
        printf("ERROR: Could not create output file '%s'!\n", destFilename);
        return 0;
    }

    // Initiate processing clock
    clock_t timeBegin = clock();

    // Initiate the heck coder
    heck_init();

    // If user requested comments, add header in the destination file
    if (addComments) {
        fputs("// Generated with VMTranslator v1.0.\n", fpdest);
        fputs("// Chakir Mrabet, 2019.\n", fpdest);
        fputs("// For Nand2Tetris, Part 2.\n", fpdest);
    }

    // Add bootstrap code
    write_file(fpdest, heck_code("init", 0, ""), "Bootstrap Code", addComments);
    write_file(fpdest, heck_code("call sys.init", 0, ""), "", 0);

    // Code the content of each on of the found files
    for (int i = 0; i < n_files; i++) {
        printf("\n> Parsing %s\n", files[i]);

        // Open source file
        FILE *fpsrc = fopen(files[i], "r");
        if (!fpsrc) {
            printf("Can't open file %s.\n", files[i]);
            return 0;
        }

        char *assembler = NULL;
        char *filename = util_get_filename(files[i]);

        // Parse each one of the lines in the current file
        int count = 0;
        char *line = (char *)malloc(LINE_MAX * sizeof(char));

        while ((line = fgets(line, LINE_MAX, fpsrc)) != NULL) {
            if (clean_line(line)) {
                assembler = heck_code(line, count, filename);
                if (assembler) {
                    write_file(fpdest, assembler, line, addComments);
                }
            }
            count++;
        }

        free(assembler);
        free(line);
        fclose(fpsrc);
    }

    // Add end of program
    write_file(fpdest, heck_code_end(), "End of program", addComments);

    heck_flush();
    fclose(fpdest);
    free(files);
    free(destFilename);

    clock_t timeEnd = clock();
    printf("\nDone in %f ms!\n", (double)(timeEnd - timeBegin) * 1000 / CLOCKS_PER_SEC);
}
