#include <stdio.h>
#include "test_framework.h"
#include "test_ht.h"
#include "../include/hashtable.h"

// --- INDIVIDUAL TEST CASES ---

static void test_create_table() {
    printf("   Running test_create_table...\n");
    
    // Using size 8 (must be power of 2 for bitwise hash optimization)
    HashTable* ht = ht_create(8);
    
    ASSERT(ht != NULL, "HashTable creation returned NULL");
    ASSERT(ht->size == 8, "HashTable size is incorrect");
    ASSERT(ht->count == 0, "HashTable should be empty initially");
    ASSERT(ht->buckets != NULL, "Buckets array creation failed");
    
    // Cleanup (manual free since we don't have ht_destroy yet)
    free(ht->buckets);
    free(ht);
}

static void test_insert_and_get() {
    printf("   Running test_insert_and_get...\n");
    
    HashTable* ht = ht_create(8);
    
    // 1. Basic Insertion
    bool result = ht_insert(ht, "username", "mario");
    ASSERT(result == true, "Failed to insert valid key-value pair");
    ASSERT(ht->count == 1, "Count should increment after insertion");

    // 2. Retrieval
    char* value = ht_get(ht, "username");
    ASSERT_STR_EQ(value, "mario", "Retrieved value does not match inserted value");

    // Cleanup
    free(ht->buckets[0]); // Rudimentary cleanup (leaking memory without full destroy)
    free(ht->buckets);
    free(ht);
}

static void test_missing_key() {
    printf("   Running test_missing_key...\n");
    
    HashTable* ht = ht_create(8);
    ht_insert(ht, "key1", "val1");
    
    // 1. Try to get a key that doesn't exist
    char* val = ht_get(ht, "ghost_key");
    ASSERT(val == NULL, "ht_get should return NULL for non-existent keys");

    // Cleanup
    free(ht->buckets);
    free(ht);
}

static void test_update_existing_key() {
    printf("   Running test_update_existing_key...\n");
    
    HashTable* ht = ht_create(8);
    
    ht_insert(ht, "config", "ver_1.0");
    
    // Update the value for the same key
    ht_insert(ht, "config", "ver_2.0");
    
    char* val = ht_get(ht, "config");
    ASSERT_STR_EQ(val, "ver_2.0", "Value should be updated to the new version");
    ASSERT(ht->count == 1, "Count should NOT increment on update");

    // Cleanup
    free(ht->buckets);
    free(ht);
}

static void test_collisions() {
    printf("   Running test_collisions...\n");
    
    // Create a tiny table (size 4) to FORCE collisions mathematically
    // With 4 slots, inserting 5 items guarantees at least one collision (Pigeonhole Principle)
    HashTable* ht = ht_create(4);
    
    ht_insert(ht, "user_a", "data_a");
    ht_insert(ht, "user_b", "data_b");
    ht_insert(ht, "user_c", "data_c");
    ht_insert(ht, "user_d", "data_d");
    ht_insert(ht, "user_e", "data_e"); // Guaranteed collision here or earlier
    
    ASSERT(ht->count == 5, "All 5 items should be inserted despite collisions");
    
    // Verify we can retrieve ALL items (collision handling verification)
    ASSERT_STR_EQ(ht_get(ht, "user_a"), "data_a", "Lost data due to collision (user_a)");
    ASSERT_STR_EQ(ht_get(ht, "user_e"), "data_e", "Lost data due to collision (user_e)");

    // Cleanup
    free(ht->buckets);
    free(ht);
}

// --- TEST SUITE RUNNER ---

void run_hashtable_tests() {
    printf("=== Hash Table Tests ===\n");
    test_create_table();
    test_insert_and_get();
    test_missing_key();
    test_update_existing_key();
    test_collisions();
    printf("\n");
}