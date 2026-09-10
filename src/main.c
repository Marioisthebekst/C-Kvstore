#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "repl.h"

#define DEFAULT_INITIAL_SIZE 16
#define DEFAULT_CAPACITY 100

int main(int argc, char *argv[]) {
    int initial_size = DEFAULT_INITIAL_SIZE;
    int capacity = DEFAULT_CAPACITY;

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--capacity") == 0) && i + 1 < argc) {
            capacity = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) && i + 1 < argc) {
            initial_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [OPTIONS]\n", argv[0]);
            printf("Options:\n");
            printf("  -c, --capacity <num>  Set maximum key capacity (default: %d)\n", DEFAULT_CAPACITY);
            printf("  -s, --size <num>      Set initial hash table bucket count (default: %d)\n", DEFAULT_INITIAL_SIZE);
            return 0;
        }
    }

    if (capacity <= 0 || initial_size <= 0) {
        fprintf(stderr, "Error: capacity and size must be positive integers.\n");
        return 1;
    }

    HashTable *ht = createTable(initial_size, capacity);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to initialize hash table.\n");
        return 1;
    }

    runRepl(ht);
    return 0;
}