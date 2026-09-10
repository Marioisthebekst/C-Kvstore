#define _POSIX_C_SOURCE 200809L
#include "db.h"
#include <stdio.h>

const float MAX_LOAD_FACTOR = 0.7;
const float MIN_LOAD_FACTOR = 0.2;
const int INITIAL_CAPACITY = 16;
const long HASH = 5381;
HashTable* createTable(int size) {
    if(size <= INITIAL_CAPACITY) {
        size = INITIAL_CAPACITY;
    }
    HashTable *table = malloc(sizeof(HashTable));
    if (table == NULL) {
        return NULL;
    }
    table->size = size;

    table->table = calloc(size, sizeof(Node*));
    if (table->table == NULL) {
        free(table);
        return NULL;
    }
    table->count = 0;

    return table;
}

static void freeNode(Node* node) {
    if(node == NULL) {
        return;
    }
    if(node->value != NULL){
        free(node->value);
    }
    if(node->key != NULL) {
        free(node->key);
    }
    node->next = NULL;
    free(node);
}

void freeTable(HashTable* ht) {
    if (ht == NULL) return;
    for (int i = 0; i< ht->size ; i++) {
        Node* current = ht->table[i];
        Node* nextNode = NULL;
        while (current != NULL) {
            nextNode = current->next;
            freeNode(current);
            current = nextNode;
        }


    }
    free(ht->table);
    free(ht);
}

static unsigned int hash(const char* key, int table_size) {
    unsigned long hash = HASH;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % table_size;
}

static int rehashTable(HashTable* ht, int newSize) {
    Node** new_table = calloc(newSize, sizeof(Node*));
    if (new_table == NULL) {
        return 0;
    }

    for (int i = 0; i < ht->size; i++) {
        Node* current = ht->table[i];
        while (current != NULL) {
            Node* nextNode = current->next;
            int index = hash(current->key, newSize);

            current->next = new_table[index];
            new_table[index] = current;

            current = nextNode;
        }
    }

    free(ht->table);
    ht->table = new_table;
    ht->size = newSize;

    return 1;
}


int resize(HashTable* ht) {
    if (ht == NULL) return 0;
    return rehashTable(ht, ht->size * 2);
}

int shrink(HashTable* ht) {
    if (ht == NULL || ht->size <= INITIAL_CAPACITY) return 0;
    return rehashTable(ht, ht->size / 2);
}
int htInsertAt(HashTable* ht, const char* key, const char* value, time_t expireAt) {
    if (ht == NULL || key == NULL || value == NULL) {
        return 0;
    }
    int index = hash(key, ht->size);
    
    Node* current = ht->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            free(current->value);
            current->value = strdup(value);
            current->expireAt = expireAt;
            return 1;
        }
        current = current->next;
    }

    Node* newNode = malloc(sizeof(Node));
    if (newNode == NULL) return 0;
    
    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->expireAt = expireAt;

    if (newNode->key == NULL || newNode->value == NULL) {
        freeNode(newNode);
        return 0;
    }

    newNode->next = ht->table[index];
    ht->table[index] = newNode;
    ht->count++;
    if ((float)ht->count / ht->size > MAX_LOAD_FACTOR) {
        resize(ht);
    }

    return 1;
}

int htInsert(HashTable* ht, const char* key, const char* value, int seconds) { 
    time_t expireAt = (seconds > 0) ? time(NULL) + seconds : 0;
    return htInsertAt(ht, key, value, expireAt);
}

int isExpired(Node* node) {
    if (node == NULL) return 0;
    return (node->expireAt != 0 && node->expireAt < time(NULL));
}

char* htGet(HashTable* ht, const char* key) {
    if (ht == NULL || key == NULL) {
        return NULL;
    }
    int index = hash(key, ht->size);
    Node* current = ht->table[index];
    while(current != NULL) {
        if(strcmp(current->key, key) == 0 ){
            if (isExpired(current)) {
                htDelete(ht, key);
                return NULL;
            }

            return strdup(current->value);
        }
        current = current->next;
    }
    return NULL;
}

int htDelete(HashTable* ht, const char* key) {
    if (ht == NULL || key == NULL) return 0;
    
    int index = hash(key, ht->size);
    Node** current_ptr = &(ht->table[index]); 

    while (*current_ptr != NULL) {
        Node* current = *current_ptr;
        if (strcmp(current->key, key) == 0) {
            *current_ptr = current->next;
            freeNode(current);
            
            ht->count--;
            if ((float)ht->count / ht->size < MIN_LOAD_FACTOR) {
                shrink(ht);
            }
            return 1;
        }
        current_ptr = &(current->next);
    }
    return 0;
}

int htIncr(HashTable* ht, const char* key, int by) {
    if (ht == NULL || key == NULL) {
        return 0;
    }

    int index = hash(key, ht->size);
    Node* current = ht->table[index];
    Node* target = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (isExpired(current)) {
                htDelete(ht, key);
            } else {
                target = current;
            }
            break;
        }
        current = current->next;
    }
    char value[20];
    int num = 0;
    if (target != NULL) {
        char* endptr;
        num = strtol(target->value, &endptr, 10);
        if (target->value == endptr || *endptr != '\0') {
            return 0;
        }
    }
    snprintf(value, sizeof(value), "%d", num + by);
    return htInsertAt(ht, key, value, key != NULL && target != NULL ? target->expireAt : 0);


}