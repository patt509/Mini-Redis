#include <stdio.h>
#include <stdlib.h>
#include "./include/hashtable.h"
#include "./include/server.h"

#define FILENAME "dump.bin"

int main() {
   HashTable* table = ht_load("dump.bin");
   if (table == NULL) {
      // Temporary error that needs to be replaced by more specific errors
      printf("Error during table loading from file: file does not exist or...\n");

      // Creating a completely new table
      int size_input;
      printf("Enter new hash table size: ");
      if (scanf("%d", &size_input) != 1) {
         printf("Invalid input.\n");
         return 1;
      }

      // Consume newline left by scanf
      while (getchar() != '\n');

      printf("Setting size to %d (or next power of 2).\n", size_input);

      table = ht_create(size_input);
      if (!table) {
         printf("Error during table allocation.");
         return 1;
      }
   }

   // Start the loop
   server_run(table);
   
   // Save the table in the file
   ht_save(table, FILENAME);
   // When server stops running
   ht_destroy(table);
   return 0;
}