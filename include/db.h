#ifndef DB_H
#define DB_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Node {
    char* key;
    char* value;
    time_t expireAt;
    struct Node* next;
} Node;

typedef struct HashTable {
    int size;
    int count;
    Node** table;
} HashTable;


HashTable* createTable(int size);
void freeTable(HashTable* ht);
int htInsert(HashTable* ht, const char* key, const char* value, int seconds);
int htInsertAt(HashTable* ht, const char* key, const char* value, time_t expireAt);
char* htGet(HashTable* ht, const char* key);
int htDelete(HashTable* ht, const char* key);
int resize(HashTable* ht);
int shrink(HashTable* ht);
int isExpired(Node* node);
int htIncr(HashTable* ht, const char* key, int by);
#endif