#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/persistence.h"

static FILE* aof_fp = NULL;

bool aof_init(const char* filename) {
    if (aof_fp) return true; // Already open

    // Open in append-binary mode.
    // "ab" creates the file if it doesn't exist.
    aof_fp = fopen(filename, "ab");
    return (aof_fp != NULL);
}

void aof_close() {
    if (aof_fp) {
        fclose(aof_fp);
        aof_fp = NULL;
    }
}

void aof_sync() {
    if (aof_fp) {
        fflush(aof_fp);
        // fsync(fileno(aof_fp)); // Optional: for stronger durability
    }
}

bool aof_log_set(const char* key, const char* value) {
    if (!aof_fp) return true; // Persistence disabled, ignore

    unsigned char op = OP_SET;
    int key_len = strlen(key) + 1; // Include null terminator
    int val_len = strlen(value) + 1;

    if (fwrite(&op, sizeof(unsigned char), 1, aof_fp) != 1) return false;
    if (fwrite(&key_len, sizeof(int), 1, aof_fp) != 1) return false;
    if (fwrite(key, key_len, 1, aof_fp) != 1) return false;
    if (fwrite(&val_len, sizeof(int), 1, aof_fp) != 1) return false;
    if (fwrite(value, val_len, 1, aof_fp) != 1) return false;

    // Flush to OS buffer immediately
    fflush(aof_fp);
    return true;
}

bool aof_log_del(const char* key) {
    if (!aof_fp) return true; // Persistence disabled

    unsigned char op = OP_DEL;
    int key_len = strlen(key) + 1;

    if (fwrite(&op, sizeof(unsigned char), 1, aof_fp) != 1) return false;
    if (fwrite(&key_len, sizeof(int), 1, aof_fp) != 1) return false;
    if (fwrite(key, key_len, 1, aof_fp) != 1) return false;

    fflush(aof_fp);
    return true;
}

HashTable* aof_load(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return NULL; // File doesn't exist

    // Read initial table size (if we decide to store it at the beginning)
    // For AOF, usually we don't store size at start because it's a stream.
    // But we need a size to create the table.
    // STRATEGY: We can either:
    // 1. Store size as the very first thing in the file (only once).
    // 2. Use a default size and resize (rehashing) - but we don't have resize yet.
    // 3. Ask user for size, then load.
    
    // Let's assume the file starts with the table size (int)
    // If the file is empty, we return NULL.
    
    int size;
    if (fread(&size, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }

    HashTable* table = ht_create(size);
    if (!table) {
        fclose(fp);
        return NULL;
    }

    unsigned char op;
    while (fread(&op, sizeof(unsigned char), 1, fp) == 1) {
        if (op == OP_SET) {
            int key_len, val_len;
            if (fread(&key_len, sizeof(int), 1, fp) != 1) break;
            
            char* key = malloc(key_len);
            fread(key, key_len, 1, fp);
            
            if (fread(&val_len, sizeof(int), 1, fp) != 1) { free(key); break; }
            
            char* val = malloc(val_len);
            fread(val, val_len, 1, fp);

            ht_insert(table, key, val);
            
            free(key);
            free(val);
        } else if (op == OP_DEL) {
            int key_len;
            if (fread(&key_len, sizeof(int), 1, fp) != 1) break;
            
            char* key = malloc(key_len);
            fread(key, key_len, 1, fp);

            ht_delete(table, key);
            free(key);
        }
    }

    fclose(fp);
    return table;
}
