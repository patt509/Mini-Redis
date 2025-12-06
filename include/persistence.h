#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdbool.h>
#include "hashtable.h"

// Operation codes for the log
typedef enum {
    OP_SET = 1,
    OP_DEL = 2
} OpCode;

/*
    Initializes the persistence module.
    Opens the file in append mode.
    Returns true if successful.
*/
bool aof_init(const char* filename);

/*
    Closes the persistence file.
*/
void aof_close();

/*
    Logs a SET operation to the file.
    Format: [OP_SET][Key Len][Key][Val Len][Val]
*/
bool aof_log_set(const char* key, const char* value);

/*
    Logs a DEL operation to the file.
    Format: [OP_DEL][Key Len][Key]
*/
bool aof_log_del(const char* key);

/*
    Loads the database by replaying the AOF log.
    Returns a new HashTable or NULL on error.
    If file doesn't exist, returns NULL (caller should create new table).
*/
HashTable* aof_load(const char* filename);

/*
    Creates a new empty AOF file with the given table size as header.
    Returns true if successful.
*/
bool aof_create_empty(const char* filename, int size);

/*
    Syncs the file buffer to disk (fsync).
    Call this if you want to be 100% sure data is on disk.
*/
void aof_sync();

#endif
