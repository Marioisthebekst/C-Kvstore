#include <stdio.h>
#include "cli.h"
#include "db.h"
#include "repl.h"

int main(int argc, char *argv[]) {
    CliOptions opts = parseArgs(argc, argv);

    if (opts.help) {
        printf("Usage: %s [OPTIONS]\n", argv[0]);
        printf("Options:\n");
        printf("  -c, --capacity <num>  Set maximum key capacity (default: %d)\n", DEFAULT_CAPACITY);
        printf("  -s, --size <num>      Set initial hash table bucket count (default: %d)\n", DEFAULT_INITIAL_SIZE);
        return 0;
    }

    if (!opts.valid) {
        fprintf(stderr, "Error: capacity and size must be positive integers.\n");
        return 1;
    }

    HashTable *ht = createTable(opts.size, opts.capacity);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to initialize hash table.\n");
        return 1;
    }

    printf("Key-Value Store initialized (size=%d, capacity=%d). Type commands (e.g., SET, GET, KEYS, EXIT):\n", opts.size, opts.capacity);

    runRepl(ht);
    return 0;
}