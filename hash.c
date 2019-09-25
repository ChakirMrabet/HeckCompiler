// HECK virtual machine bytecode to ASM parser
// Nand to Tetris, Part 2
// Chakir Mrabet, 2019
// hash.c
// Simple hash table implementation

#include "hash.h"
#include <stdlib.h>
#include <string.h>

void table_init(table *t) {
    t->count = 0;
    t->first = NULL;
    t->last = NULL;
}

// Add a new value in the table
void table_push(table *t, char *key, char *value) {
    size_t keyLen = strlen(key) + 1;
    size_t valueLen = strlen(value) + 1;

    table_node *n = (table_node *)malloc(sizeof(table_node));
    n->key = (char *)malloc(keyLen * sizeof(char));
    n->value = (char *)malloc(valueLen * sizeof(char));
    n->next = NULL;
    n->previous = NULL;

    strcpy(n->key, key);
    strcpy(n->value, value);

    if (!t->first) {
        t->first = n;
    }

    if (t->last) {
        t->last->next = n;
        n->previous = t->last;
    }

    t->last = n;
    t->count++;
}

// Get number of items
int table_count(table *t) { return t->count; }

// Get key and value at pos n
table_node *table_at(table *t, int n) {
    table_node *p = t->first;
    int count = 0;

    while (p != NULL && count < n) {
        p = p->next;
        count++;
    }
    return p;
}

// Get first item with given key
char *table_value_for(table *t, char *key) {
    table_node *p = t->first;

    while (p != NULL) {
        if (strcmp(p->key, key) == 0) {
            return p->value;
        }
        p = p->next;
    }
    return NULL;
}

// Free all the memory allocated
void table_free(table *t) {
    table_node *p = t->first;

    // Free memory allocated for each item
    while (p->next != NULL) {
        p = p->next;
        free(p->previous->key);
        free(p->previous->value);
        free(p->previous);
    }
    if (p) {
        free(p->key);
        free(p->value);
        free(p);
    }

    table_init(t);
}
