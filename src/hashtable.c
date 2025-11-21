#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Including corresponding header file
#include "../include/hashtable.h"

HashTable* ht_create(int size) {
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
   if(!buckets) {
      free(table);
      return NULL;
   }
   table->buckets = buckets;
   table->size = size;
   table->count = 0;

   return table;
}

ulong hash(char* value) {
   return 0;
}

Node* ht_create_pair(char* value, char* key) {
   // Value or key not valid
   if(!value || !key) {
      return NULL;
   }

   // Reserving memory for value and key
   char* newValue = malloc(strlen(value) * sizeof(char));
   if (!newValue) {
      return NULL;
   }
   char* newKey = malloc(strlen(key) * sizeof(char));
   if (!newKey) {
      free(newValue);
      return NULL;
   }
   strcpy(newValue, value);
   strcpy(newKey, key);

   Node* node = malloc(sizeof(Node));
   if(!node) {
      free(newValue);
      free(newKey);
      return NULL;
   }
   node->value = newValue;
   node->key = newKey;
   node->next = NULL;

   return node;
}

bool ht_insert(HashTable* table, char* key, char* value) {
   
}