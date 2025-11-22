#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include <stdbool.h>

// List node (for collisions)
typedef struct Node {
   char* key;
   char* value;
   uint64_t rawHash;    // Storing row hash in case of future changes to the table
   struct Node *next;   // Pointer to the next node of the collision bucket
} Node;

// Hash table struct
typedef struct HashTable {
   Node** buckets;   // Array of pointers
   int size;         // Max array size
   int count;        // Total elements inserted
} HashTable;

/*
   Function that creates the table.

   It accepts the initial size of the table, allocates
   the buckets. It sets every bucket to NULL because
   non initialized pointers can point to random
   data and this would mess up the program.
*/
HashTable* ht_create (int size);

/*
   Hash function that produces the corresponding index.

   Accepts only the string value.
   - the specific hash function needs to be defined -
*/
unsigned long hash (char* value);

/*
   Helper for the insert function.

   Accepts input value and key.
   Allocates a new node, allocates memory for the value
   and for the key (deep copy) and saves them in the node
   fields.
   Sets the next pointer of the node to NULL and return
   the pointer to the just created node.
*/
Node* ht_create_pair (char* value, char* key);

/*
   Function that inserts a node into the table.

   Accept a pointer to the table, the key and the value.
   Calls the hash function passing the key to it to obtain
   the corresponding index. Then it goes to the bucket at
   the found index and iterates through the bucket until:
   1) A node with the same key is found: in this case the
      previuos node has to be freed and replaced with
      the new node.
   2) If a node with the same key is not found, the new
      node will be insterted at the head of the bucket.
   It calls the helper to create a new node. The "next"
   pointer of the new node will point to the previous
   first node and the bucket needs to point to the new node.
   Then increments the counter of total elements.
*/
bool ht_insert (HashTable* table, char* key, char* value);

/*
   Function that returns a pointer to the value the
   searched node is storing (node->value).
   If not found, it will return NULL.

   Accepts a pointer to the table and the key.
   Calls the hash function to calculate the index of the
   bucket to check and iterates though that bucket:
   confronts every key with string comparing functions.
   If a match is found, the function will return char* value.
   If the whole bucket does not contain the key, return NULL.
*/
char* ht_get (HashTable* table, char* key);

#endif