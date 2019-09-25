// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// utils.h
// Utility library

// Returns 1 if the string is empty, 0 otherwise
int util_is_empty(char *);

// Removes all spaces at the left of the given string
void util_ltrim(char *);

// Removes all spaces at the right of the given string
void util_rtrim(char *);

// Removes all end carriage and new line characters
void util_trim_nl(char *);

// Removes comments
void util_remove_comments(char *);

// Returns file name with its extension
char *util_get_filename(char *);

// Gets the extension for the given filename
char *util_get_file_extension(char *);

// Returns an .asm file name from a .vm filename
char *util_get_dest_filename(char *);

// Returns source name without the base directory
char *util_get_filename_base(char *);

// Cleans the given string by removing comments, empty spaces,
// new line and return carriage characters
int clean_line(char *);
