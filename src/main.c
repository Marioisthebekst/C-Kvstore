#include <stdio.h>
#include "repl.h"

int main(void) {
    HashTable* ht = createTable(5);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to initialize hash table\n");
        return 1;
    }

    printf("Key-Value Store initialized. Type commands (e.g., SET, GET, KEYS, EXIT):\n");

    runRepl(ht);

    return 0;
}