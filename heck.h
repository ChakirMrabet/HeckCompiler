// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// heck.h
// VM to ASM coder

// Inits the HECK coder
void heck_init(void);

// Flushes memory used by the HECK coder
void heck_flush();

// Returns the coded ASM string for the given string
char *heck_code(char *, int, char *);

// Returns a coded ASM string with the end of a program
char *heck_code_end(void);
