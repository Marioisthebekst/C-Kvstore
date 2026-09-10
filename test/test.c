#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "db.h"
#include "repl.h"
#include "test.h"

void testCreateFree(void) {
    printf("--- test_create_free ---\n");

    HashTable* ht = createTable(10);
    assert(ht != NULL);
    assert(ht->size == 16);
    assert(ht->count == 0);
    freeTable(ht);

    HashTable* ht2 = createTable(100);
    assert(ht2 != NULL);
    assert(ht2->size == 100);
    freeTable(ht2);

    freeTable(NULL);

    printf("[PASS] create_free\n");
}

void testInsertGet(void) {
    printf("--- test_insert_get ---\n");

    HashTable* ht = createTable(10);
    assert(ht != NULL);

    assert(htInsertAt(NULL, "key", "val", 0) == 0);
    assert(htInsertAt(ht, NULL, "val", 0) == 0);
    assert(htInsertAt(ht, "key", NULL, 0) == 0);
    assert(htGet(ht, NULL) == NULL);
    assert(htGet(NULL, "key") == NULL);
    printf("[PASS] NULL handling\n");

    assert(htInsertAt(ht, "user", "Alice", 0) == 1);
    assert(htInsertAt(ht, "role", "Admin", 0) == 1);

    char* val1 = htGet(ht, "user");
    assert(val1 != NULL && strcmp(val1, "Alice") == 0);
    free(val1);

    char* val2 = htGet(ht, "role");
    assert(val2 != NULL && strcmp(val2, "Admin") == 0);
    free(val2);
    printf("[PASS] basic insert/get\n");

    assert(htGet(ht, "missing") == NULL);
    printf("[PASS] missing key returns NULL\n");

    assert(htInsertAt(ht, "user", "Bob", 0) == 1);
    char* updated = htGet(ht, "user");
    assert(updated != NULL && strcmp(updated, "Bob") == 0);
    free(updated);
    assert(ht->count == 2);
    printf("[PASS] overwrite existing key\n");

    freeTable(ht);
    printf("[PASS] insert_get\n");
}

void testDelete(void) {
    printf("--- test_delete ---\n");

    HashTable* ht = createTable(10);
    assert(ht != NULL);

    assert(htDelete(NULL, "key") == 0);
    assert(htDelete(ht, NULL) == 0);
    assert(htDelete(ht, "ghost") == 0);
    printf("[PASS] delete NULL/missing handling\n");

    assert(htInsertAt(ht, "fruit", "Apple", 0) == 1);
    assert(htDelete(ht, "fruit") == 1);
    assert(htGet(ht, "fruit") == NULL);
    assert(htDelete(ht, "fruit") == 0);
    printf("[PASS] single delete\n");

    assert(htInsertAt(ht, "k1", "v1", 0) == 1);
    assert(htInsertAt(ht, "k2", "v2", 0) == 1);
    assert(htInsertAt(ht, "k3", "v3", 0) == 1);

    assert(htDelete(ht, "k3") == 1);
    assert(htGet(ht, "k3") == NULL);
    assert(htDelete(ht, "k1") == 1);
    assert(htGet(ht, "k1") == NULL);

    char* v2 = htGet(ht, "k2");
    assert(v2 != NULL && strcmp(v2, "v2") == 0);
    free(v2);

    freeTable(ht);
    printf("[PASS] delete\n");
}

void testResizeShrink(void) {
    printf("--- test_resize_shrink ---\n");

    HashTable* ht = createTable(4);
    assert(ht != NULL);
    assert(ht->size == 16);

    char key[16];
    for (int i = 0; i < 12; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        assert(htInsertAt(ht, key, "v", 0) == 1);
    }
    assert(ht->count == 12);
    assert(ht->size == 32);
    printf("[PASS] resize triggers past MAX_LOAD_FACTOR (16 -> 32)\n");

    for (int i = 0; i < 12; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        char* v = htGet(ht, key);
        assert(v != NULL && strcmp(v, "v") == 0);
        free(v);
    }
    printf("[PASS] all keys survive resize\n");

    for (int i = 0; i < 12; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        htDelete(ht, key);
    }
    assert(ht->count == 0);
    printf("[PASS] shrink does not go below INITIAL_CAPACITY (size=%d)\n", ht->size);
    assert(ht->size >= 16);

    freeTable(ht);
    printf("[PASS] resize_shrink\n");
}

void testTtlRelative(void) {
    printf("--- test_ttl_relative ---\n");

    HashTable* ht = createTable(10);
    assert(ht != NULL);

    assert(htInsert(ht, "session", "abc123", 100) == 1);
    char* val = htGet(ht, "session");
    assert(val != NULL && strcmp(val, "abc123") == 0);
    free(val);
    printf("[PASS] htInsert with future TTL is readable\n");

    assert(htInsert(ht, "permanent", "stays", -1) == 1);
    val = htGet(ht, "permanent");
    assert(val != NULL && strcmp(val, "stays") == 0);
    free(val);
    printf("[PASS] non-positive seconds means no TTL (permanent entry)\n");

    freeTable(ht);
    printf("[PASS] ttl_relative\n");
}

void testTtlAbsolute(void) {
    printf("--- test_ttl_absolute ---\n");

    HashTable* ht = createTable(10);
    assert(ht != NULL);

    time_t future = time(NULL) + 1000;
    assert(htInsertAt(ht, "future_key", "still_here", future) == 1);
    char* val = htGet(ht, "future_key");
    assert(val != NULL && strcmp(val, "still_here") == 0);
    free(val);
    printf("[PASS] future absolute timestamp is readable\n");

    time_t past = time(NULL) - 1000;
    assert(htInsertAt(ht, "past_key", "long_gone", past) == 1);
    assert(isExpired(NULL) == 0);
    assert(htGet(ht, "past_key") == NULL);
    printf("[PASS] past absolute timestamp is lazily expired\n");

    freeTable(ht);
    printf("[PASS] ttl_absolute\n");
}

void testIncrDecr(void) {
    printf("--- test_incr_decr ---\n");

    HashTable* ht = createTable(10);
    assert(ht != NULL);

    assert(htIncr(ht, "counter", 1) == 1);
    char* v = htGet(ht, "counter");
    assert(v != NULL && strcmp(v, "1") == 0);
    free(v);
    printf("[PASS] INCR auto-creates missing key at 0\n");

    assert(htIncr(ht, "counter", 1) == 1);
    v = htGet(ht, "counter");
    assert(v != NULL && strcmp(v, "2") == 0);
    free(v);

    assert(htIncr(ht, "counter", -5) == 1);
    v = htGet(ht, "counter");
    assert(v != NULL && strcmp(v, "-3") == 0);
    free(v);
    printf("[PASS] INCR/DECR arithmetic\n");

    assert(htInsertAt(ht, "word", "hello", 0) == 1);
    assert(htIncr(ht, "word", 1) == 0);
    printf("[PASS] INCR on non-numeric value fails cleanly\n");

    freeTable(ht);
    printf("[PASS] incr_decr\n");
}

void testReplIntegration(void) {
    printf("--- test_repl_integration ---\n");

    FILE* in = fopen("test_repl_input.txt", "w");
    assert(in != NULL);

    fprintf(in, "SET key1 value1\n");
    fprintf(in, "GET key1\n");
    fprintf(in, "SET greeting \"Hello World from C\"\n");
    fprintf(in, "GET greeting\n");
    fprintf(in, "SETEX temp hello 100\n");
    fprintf(in, "GET temp\n");
    fprintf(in, "SET broken \"Unclosed string\n");
    fprintf(in, "INCR counter\n");
    fprintf(in, "INCR counter\n");
    fprintf(in, "DECR counter\n");
    fprintf(in, "EXIST key1\n");
    fprintf(in, "EXIST ghost\n");
    fprintf(in, "COUNT\n");
    fprintf(in, "KEYS\n");
    fprintf(in, "DELETE key1\n");
    fprintf(in, "GET key1\n");
    fprintf(in, "   \n");
    fprintf(in, "INVALID_COMMAND\n");
    fprintf(in, "EXIT\n");
    fclose(in);

    FILE* redirected = freopen("test_repl_input.txt", "r", stdin);
    assert(redirected != NULL);

    HashTable* ht = createTable(10);
    assert(ht != NULL);

    printf("=== REPL session start ===\n");
    runRepl(ht);
    printf("=== REPL session end ===\n");

    remove("test_repl_input.txt");
    printf("[PASS] repl_integration\n");
}

void testSaveLoad(void) {
    printf("--- test_save_load ---\n");
    const char* filename = "test_persist.txt";

    HashTable* ht1 = createTable(10);
    assert(ht1 != NULL);
    assert(htInsertAt(ht1, "plain", "no_ttl", 0) == 1);
    assert(htInsert(ht1, "relative", "expires_later", 500) == 1);
    time_t absolute = time(NULL) + 1000;
    assert(htInsertAt(ht1, "absolute", "also_later", absolute) == 1);

    SAVE(ht1, (char*)filename);
    freeTable(ht1);

    HashTable* ht2 = createTable(10);
    assert(ht2 != NULL);
    LOAD(ht2, (char*)filename);

    char* v1 = htGet(ht2, "plain");
    assert(v1 != NULL && strcmp(v1, "no_ttl") == 0);
    free(v1);

    char* v2 = htGet(ht2, "relative");
    assert(v2 != NULL && strcmp(v2, "expires_later") == 0);
    free(v2);

    char* v3 = htGet(ht2, "absolute");
    assert(v3 != NULL && strcmp(v3, "also_later") == 0);
    free(v3);

    printf("[PASS] SAVE/LOAD round-trip preserves plain, SETEX and SETAT entries\n");

    freeTable(ht2);
    remove(filename);
    printf("[PASS] save_load\n");
}