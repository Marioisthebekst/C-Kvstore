#include <stdio.h>
#include "repl.h"
#include "db.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define BUFFER_SIZE 256

int SET(HashTable* ht, int mode) {
    char* key = strtok(NULL, " ");
    if (key == NULL) {
        printf("ERROR: Missing key\n");
        return 0;
    }

    char* rest = strtok(NULL, "");
    if (rest == NULL) {
        printf("ERROR: Missing value\n");
        return 0;
    }

    while (*rest == ' ') {
        rest++;
    }

    if (*rest == '\0') {
        printf("ERROR: Missing value\n");
        return 0;
    }

    char* val = NULL;
    char* afterVal = NULL;

    if (*rest == '"') {
        val = rest + 1;
        char* closingQuote = strchr(val, '"');
        if (closingQuote == NULL) {
            printf("ERROR: Unclosed quote\n");
            return 0;
        }
        *closingQuote = '\0';
        afterVal = closingQuote + 1;
    }
    else {
        val = rest;
        char* space = strchr(val, ' ');
        if (space != NULL) {
            *space = '\0';
            afterVal = space + 1;
        } else {
            afterVal = val + strlen(val);
        }
    }

    time_t expireAt = 0;

    if (mode != 0) {
        while (*afterVal == ' ') {
            afterVal++;
        }
        if (*afterVal == '\0') {
            printf(mode == 1 ? "ERROR: Missing seconds\n" : "ERROR: Missing timestamp\n");
            return 0;
        }

        long number = atol(afterVal);
        if (number <= 0) {
            printf(mode == 1 ? "ERROR: Seconds must be a positive integer\n" : "ERROR: Timestamp must be a positive integer\n");
            return 0;
        }

        expireAt = (mode == 1) ? time(NULL) + number : (time_t)number;
    }

    if (htInsertAt(ht, key, val, expireAt) != 1) {
        printf("ERROR: Failed to set key\n");
        return 0;
    }
    return 1;
}

void GET(HashTable* ht) {
    char* key = strtok(NULL, " ");
    if (key == NULL) {
        printf("ERROR: Usage: GET <key>\n");
    } else {
        char* value = htGet(ht, key);
        if (value != NULL) {
            printf("%s\n", value);
            free(value);
        } else {
            printf("NULL\n");
        }
    }
}


void DELETE(HashTable* ht) {
    char* key = strtok(NULL, " ");

    if (key == NULL) {
        printf("ERROR: Usage: DELETE <key>\n");
    } else {
        if (htDelete(ht, key)) {
            printf("Key Deleted!\n");
        } else {
            printf("ERROR: Key not found\n");
        }
    }
}

void SAVE(HashTable* ht, char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("ERROR: Could not open file for writing\n");
        return;
    }

    for(int i=0; i < ht->size; i++) {
        Node* current = ht->table[i];
        while(current != NULL) {
            if (current->expireAt == 0) {
                fprintf(file, "SET %s \"%s\"\n", current->key, current->value);
            } else {
                fprintf(file, "SETAT %s \"%s\" %ld\n", current->key, current->value, (long)current->expireAt);
            }
            current = current->next;
        }
    }
    fclose(file);
    printf("Table saved successfully!\n");
}

void LOAD(HashTable* ht, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR: Could not open file for reading\n");
        return;
    }
    char line[BUFFER_SIZE];

    while (fgets(line, BUFFER_SIZE, file) != NULL) {
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        char* cmd = strtok(line, " ");
        if (cmd == NULL) continue;

        if (strcmp(cmd, "SET") == 0) {
            SET(ht, 0);
        }
        else if (strcmp(cmd, "SETEX") == 0) {
            SET(ht, 1);
        }
        else if (strcmp(cmd, "SETAT") == 0) {
            SET(ht, 2);
        }
    }
    fclose(file);
    printf("Table loaded successfully!\n");
}

static void helper(HashTable* ht, void (*func)(HashTable*, char*)) {
    char* filename = strtok(NULL, " ");
    if (filename == NULL) {
        printf("ERROR: Usage: <filename>\n");
    } else {
        func(ht, filename);
    }
}

void KEYS(HashTable *table) {
    if(table == NULL) {
        return;
    }
    for (int i = 0; i < table->size; i++) {
        Node *current = table->table[i];
        while (current != NULL) {
            if (!isExpired(current)) {
                printf("Key: %s\n", current->key);
            }
            current = current->next;
        }
    }
}


void EXIST(HashTable* ht) {
    char* key = strtok(NULL, " ");
    if (key == NULL) {
        printf("ERROR: Usage: EXIST <key>\n");
    } else {
        char* val = htGet(ht, key);
        if (val != NULL) {
            printf("Key exists!\n");
            free(val);
        } else {
            printf("ERROR: Key not found\n");
        }
    }
}

static void handleIncrDecr(HashTable* ht, int by, const char* cmd_name) {
    char* key = strtok(NULL, " \t\n");
    if (key == NULL) {
        printf("ERROR: Usage: %s <key>\n", cmd_name);
        return;
    }

    if (htIncr(ht, key, by)) {
        char* val = htGet(ht, key);
        if (val != NULL) {
            printf("(integer) %s\n", val);
            free(val);
        }
    } else {
        printf("ERROR: value is not an integer or out of range\n");
    }
}

void INCR(HashTable* ht) {
    handleIncrDecr(ht, 1, "INCR");
}

void DECR(HashTable* ht) {
    handleIncrDecr(ht, -1, "DECR");
}

void runRepl(HashTable* ht) {
    while(1) {
        char input[BUFFER_SIZE]; 
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        int len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
            }
        char* command = strtok(input, " ");
        if(command != NULL) {
            if(strcmp(command, "SET") == 0) {
                if(SET(ht, 0)) {
                    printf("Key Set Successfully!\n");
                }
                continue;
            }
            if(strcmp(command, "SETEX") == 0) {
                if(SET(ht, 1)) { 
                    printf("Key Set with TTL Successfully!\n");
                }
                continue;
            }
            if(strcmp(command, "SETAT") == 0) {
                if(SET(ht, 2)) {
                    printf("Key Set with absolute expiry Successfully!\n");
                }
                continue;
            }
            if(strcmp(command, "GET") == 0) {
                GET(ht);
                continue;
            }
            if(strcmp(command, "DELETE") == 0) {
                DELETE(ht);
                continue;
            }
            if(strcmp(command, "EXIT") == 0) {
                break;
            }
            if(strcmp(command, "SAVE") == 0) {
                helper(ht, SAVE);
                continue;
            }

            if (strcmp(command, "LOAD") == 0) {
                helper(ht, LOAD);
                continue;
            }

            if (strcmp(command, "KEYS") == 0) {
                KEYS(ht);
                continue;
            }

            if (strcmp(command, "COUNT") == 0) {
                printf("Total keys: %d\n", ht->count);
                continue;
            }

            if (strcmp(command, "EXIST") == 0) {
                EXIST(ht);
                continue;
            }

            if (strcmp(command, "DECR") == 0) {
                DECR(ht);
                continue;
            }
            if (strcmp(command, "INCR") == 0) {
                INCR(ht);
                continue;
            }

            else {
                printf("ERROR: Command not found\n");
            }
        }
    }
    freeTable(ht);
}