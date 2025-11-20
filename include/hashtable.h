#ifndef HASHTABLE_H
#define HASHTABLE_H

// List node (for collisions)
typedef struct Node {
   char* key;
   char* value;

   // Pointer to the next node of the collision bucket
   struct Node *next;
} Node;

// Hash table struct
typedef struct HashTable {
   Node** buckets;   // Array of pointers
   int size;         // Max array size
   int count;        // Total elements inserted
} HashTable;

// Function that creates the table and initialize every string to NULL
HashTable* hash_table_create(int size);

// Function that inserts an element into the table
// It creates the node with the key and the value and then calculates the
// hash index to insert the node at
bool hash_table_insert(HashTable* table, char* key, char* value);

#endif