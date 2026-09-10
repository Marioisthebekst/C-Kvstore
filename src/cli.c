#include <stdlib.h>
#include <string.h>
#include "cli.h"

CliOptions parseArgs(int argc, char *argv[]) {
    CliOptions opts;
    opts.size = DEFAULT_INITIAL_SIZE;
    opts.capacity = DEFAULT_CAPACITY;
    opts.help = 0;
    opts.valid = 1;

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--capacity") == 0) && i + 1 < argc) {
            opts.capacity = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) && i + 1 < argc) {
            opts.size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            opts.help = 1;
            return opts;
        }
    }

    if (opts.capacity <= 0 || opts.size <= 0) {
        opts.valid = 0;
    }

    return opts;
}