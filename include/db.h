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

    struct Node *lruPrev;
    struct Node *lruNext;
} Node;

typedef struct HashTable {
    int size;
    int count;
    int capacity;
    Node** table;
    Node *lruHead;
    Node *lruTail;
} HashTable;


HashTable* createTable(int size, int capacity);
void freeTable(HashTable* ht);
int htInsert(HashTable* ht, const char* key, const char* value, int seconds);
int htInsertAt(HashTable* ht, const char* key, const char* value, time_t expireAt);
char* htGet(HashTable* ht, const char* key);
int htDelete(HashTable* ht, const char* key);
int resize(HashTable* ht);
int shrink(HashTable* ht);
int isExpired(Node* node);
int htIncr(HashTable* ht, const char* key, int by);
void lruPurgeExpiredTail(HashTable *ht, int max_checks);
#endif