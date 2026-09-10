#ifndef CLI_H
#define CLI_H
 
#define DEFAULT_INITIAL_SIZE 16
#define DEFAULT_CAPACITY 100
 
typedef struct {
    int size;
    int capacity;
    int help;
    int valid;
} CliOptions;
 
CliOptions parseArgs(int argc, char *argv[]);
 
#endif
 