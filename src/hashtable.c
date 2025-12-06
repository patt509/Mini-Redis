#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Including corresponding header file
#include "../include/hashtable.h"

// Constants definition for hash function
#define FNV_OFFSET_BASIS 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

/* 
   Private helper function that transform the value
   inserted by the user in a power of 2 to make
   bit-wise logic work.
*/
static int next_power_of_2 (int n) {
   // If the dim is 0 or negative
   if (n < 1) {
      return 1;
   }

   int power = 1;
   while (power < n) {
      // Bit-wise shift operation that
      // multiplies by two, extremely efficient
      power <<= 1;
   }

   return power;
}

HashTable* ht_create (int size) {
   int original_size = size;
   size = next_power_of_2(size);

   if (size != original_size) {
      printf("Notice: Hash table size adjusted from %d to %d (must be power of 2)\n", original_size, size);
   }

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
static uint64_t hash (char* key) {
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

Node* ht_create_pair (char* key, char* value, uint64_t hash) {
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
   node->rawHash = hash;
   node->next = NULL;
   return node;
}

bool ht_insert (HashTable* table, char* key, char* value) {
   // Get the hash stored in the node and normalize it
   // to the size of the table

   // WARNING: this AND normalization only works if table size is a power of 2!
   uint64_t h = hash(key);
   unsigned int index = h & (table->size - 1);   // Get the index using AND bit-wise operation
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
   Node* newNode = ht_create_pair(key, value, h);
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

bool ht_delete (HashTable* table, char* key) {
   if (table == NULL || key == NULL) {
      return false;
   }

   unsigned int index = hash(key) & (table->size - 1);
   Node* tmp = table->buckets[index];

   // If the bucket is empty, the key is definitely not here
   if (tmp == NULL) {
      return false;
   }

   // In case the first node of the bucket
   // stores the value the function is searching for
   if (strcmp(tmp->key, key) == 0) {
      table->buckets[index] = tmp->next;

      free(tmp->key);
      free(tmp->value);      
      free(tmp);

      return true;
   }

   // In case the searched value is not stored
   // in the first node of the bucket
   Node* next = tmp->next;
   while (next) {
      if (strcmp(next->key, key) == 0) {
         tmp->next = next->next;

         free(next->key);
         free(next->value);
         free(next);
         table->count--;

         return true;
      }

      tmp = next;
      next = next->next;
   }

   return false;
}

void ht_destroy (HashTable* table) {
   if (!table) {
      return;
   }

   for (int i = 0; i < table->size; i++) {
      Node* tmp = table->buckets[i];
      Node* prev = NULL;

      while (tmp != NULL) {
         // Set every pointer to NULL and
         // deallocate the node
         free(tmp->key);
         free(tmp->value);
         prev = tmp;

         tmp = tmp->next;
         free(prev);
      }
   }

   free(table->buckets);
   free(table);
}

bool ht_save (HashTable* table, const char* filename) {
   // Open the file in write binary mode
   FILE* fp = fopen(filename, "wb");
   if (!fp) {
      // Return 1, it means an error occured
      // trying to save the table
      return false;
   }

   // The first byte needs to be the size of the table
   fwrite(&table->size, sizeof(table->size), 1, fp);
   
   // Double cycle, iterate through the array of buckets,
   // and for every bucket iterates through the nodes
   for (int i = 0; i < table->size; i++) {
      Node* tmp = table->buckets[i];

      while (tmp != NULL) {
         // Save key size (with '\0'), key,
         // value size (with '\0') and value
         int tmp_size;
         tmp_size = strlen(tmp->key) + 1; // '\0' character
         // Write the size of the key
         fwrite(&tmp_size, sizeof(int), 1, fp);
         // Write the key itself
         fwrite(tmp->key, tmp_size, 1, fp); // '\0' character
         // Write the size of the value
         tmp_size = strlen(tmp->value) + 1;
         fwrite(&tmp_size, sizeof(int), 1, fp);
         // Write the value itself
         fwrite(tmp->value, tmp_size, 1, fp);

         tmp = tmp->next;
      }
   }

   // Close the file stream
   fclose(fp);
   return true;
}

// Helper function for ht_load: takes ownership of key and value pointers
bool ht_insert_own(HashTable* table, char* key, char* value) {
   uint64_t h = hash(key);
   unsigned int index = h & (table->size - 1);
   Node* tmp = table->buckets[index];

   // Check if the key already exists
   while (tmp != NULL) {
      if (strcmp(tmp->key, key) == 0) {
         // It means there already is a node with the
         // same key. In this case the function frees
         // the new key ad the previuos value and assignes
         // the new value to the old node
         free(key);
         free(tmp->value);
         tmp->value = value;
         // The count remains the same
         return true;
      }

      tmp = tmp->next;
   }

   // If the function gets here it means the key does not exist
   // In this case it allocates the memory for a new node
   Node* newNode = malloc(sizeof(Node));
   if (!newNode) {
      return false;
   }
   newNode->key = key;
   newNode->value = value;
   newNode->rawHash = h;
   newNode->next = table->buckets[index];
   table->buckets[index] = newNode;
   table->count++;
   return true;
}

HashTable* ht_load(const char* filename) {
   FILE* fp = fopen(filename, "rb");
   if (!fp) {
      return NULL;
   }

   // Get table size from the first 4 bytes (int size)
   int table_size;
   fread(&table_size, sizeof(int), 1, fp);
   if (table_size <= 0) {
      // Return NULL if table size is not valid
      return NULL;
   }

   HashTable* table = ht_create(table_size);
   if (table == NULL) {
      return NULL;
   }

   int key_size;
   int value_size;
   char* key = NULL;
   char* value = NULL;
   // If the size of the the key is correclty read
   // it means the node is valid and the function can
   // read the key, the value size and the value
   // without checking for errors
   while (fread(&key_size, sizeof(int), 1, fp) == 1) {
      // Allocate the exact amount of bytes for the key
      key = malloc(key_size);
      // Get the key from the file
      fread(key, key_size, 1, fp);

      // Get the size of the value
      fread(&value_size, sizeof(int), 1, fp);
      // Allocate the exact amount of bytes for the value
      value = malloc(value_size);
      // Get the value from the file
      fread(value, value_size, 1, fp);

      // Call helper function, read comments for details
      bool done = ht_insert_own(table, key, value);

      if (!done) {
         ht_destroy(table);
         fclose(fp);
         return NULL;
      }
   }

   fclose(fp);
   return table;
}