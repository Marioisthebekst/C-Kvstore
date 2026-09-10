#ifndef REPL_H
#define REPL_H
#include "db.h"

int SET(HashTable* ht, int mode);
void GET(HashTable* ht);
void DELETE(HashTable* ht);
void SAVE(HashTable* ht, char* filename);
void LOAD(HashTable* ht, char* filename);
void KEYS(HashTable* ht);
void EXIST(HashTable* ht);
void INCR(HashTable* ht);
void DECR(HashTable* ht);
void runRepl(HashTable* ht);
#endif