#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Including corresponding header file
#include "../include/hashtable.h"

// Constants definition for hash function
#define FNV_OFFSET_BASIS 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

HashTable* ht_create (int size) {
   // Allocate space for the table
   HashTable* table = malloc(sizeof(HashTable));
   // Checks error during the allocation
   if (!table) {
      return NULL;
   }

   // Allocate space for every bucket and sets each
   // of them to NULL (calloc does this automatically)
   Node** buckets = calloc(size, sizeof(Node*));
   // Checks error during the allocation
   if (!buckets) {
      free(table);
      return NULL;
   }
   table->buckets = buckets;
   table->size = size;
   table->count = 0;

   return table;
}

// Implementation of the FNV-1a for non-cryptographic ht
uint64_t hash (char* key) {
   uint64_t hash = FNV_OFFSET_BASIS;

   // For every character of the char* value string
   // While the value of c is not '\0'
   for (const char* c = key; *c; c++) {
      /*
         Double cast (char can be signed (127 to -128) or unsigned (0 to 255),
         depending on the compiler)
         The first cast say to the compiler we dont care about the sign
         so it will treat the char like a number from 0 to 255.
         The second cast is for the type the function actually
         needs to return, the char type has to be extended to a
         64 bits integer in order for the XOR bit-wise operation to
         work properly.
      */
      hash = hash ^ (uint64_t)(unsigned char)(*c);   // Bitwise XOR operation
      hash = hash * FNV_PRIME;                       // Multiply with FNV_PRIME, big prime number
   }

   return hash;
}

Node* ht_create_pair (char* key, char* value) {
   // Value or key not valid
   if (!value || !key) {
      return NULL;
   }

   // Reserving memory for value and key
   char* newValue = strdup(value);
   if (!newValue) {
      return NULL;
   }
   char* newKey = strdup(key);
   if (!newKey) {
      free(newValue);
      return NULL;
   }

   Node* node = malloc(sizeof(Node));
   if (!node) {
      free(newValue);
      free(newKey);
      return NULL;
   }
   node->value = newValue;
   node->key = newKey;
   node->rawHash = hash(key);
   node->next = NULL;
   return node;
}

bool ht_insert (HashTable* table, char* key, char* value) {
   // Get the hash stored in the node and normalize it
   // to the size of the table

   // WARNING: this AND normalization only works if table size is a power of 2!
   unsigned int index = hash(key) & (table->size - 1);   // Get the index using AND bit-wise operation
   Node* tmp = table->buckets[index];

   while (tmp != NULL) {
      // Case 1: If a node with the same key is found...
      if (strcmp(tmp->key, key) == 0) {
         // ...free the previous value and replace it
         // with the new one

         // No need to create a new node, just modify the
         // current value because we verified the key is
         // exactly the same (count is not incremented)
         free(tmp->value);
         tmp->value = strdup(value);
         return true;
      }

      tmp = tmp->next;
   }

   // Case 2: If a node with the same key is NOT found
   // the node is inserted at the head of the bucket
   Node* newNode = ht_create_pair(key, value);
   if (!newNode) {
      return false;
   }
   newNode->next = table->buckets[index];
   table->buckets[index] = newNode;
   table->count++;
   return true;
}

char* ht_get (HashTable* table, char* key) {
   unsigned int index = hash(key) & (table->size - 1);
   Node* tmp = table->buckets[index];

   // Iterates through the bucket until the key is found
   while (tmp != NULL) {
      if (strcmp(tmp->key, key) == 0) {
         return tmp->value;
      }

      tmp = tmp->next;
   }

   // If not found return NULL
   return NULL;
}