// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// hash.h
// Simple hash table implementation

#include <stdio.h>

typedef struct table_node {
    char *key;
    char *value;
    struct table_node *previous;
    struct table_node *next;
} table_node;

typedef struct table {
    int count;
    struct table_node *first;
    struct table_node *last;
} table;

void table_init(table *);
void table_push(table *, char *, char *);
int table_count(table *);
table_node *table_at(table *, int);
char *table_value_for(table *, char *);
void table_free(table *);
