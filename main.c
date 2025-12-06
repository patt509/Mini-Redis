#include <stdio.h>
#include <stdlib.h>
#include "./include/hashtable.h"
#include "./include/server.h"
#include "./include/persistence.h"

#define FILENAME "dump.aof"

int main() {
   printf("=== Mini-Redis Server ===\n");
   
   // Ask for persistence
   char persistence_choice;
   bool use_persistence = false;
   bool valid_choice = false;

   do {
      printf("Enable persistence? (y/n): ");
      if (scanf(" %c", &persistence_choice) == 1) {
         if (persistence_choice == 'y' || persistence_choice == 'Y') {
            use_persistence = true;
            valid_choice = true;
         } else if (persistence_choice == 'n' || persistence_choice == 'N') {
            use_persistence = false;
            valid_choice = true;
         } else {
            printf("Invalid choice. Please enter 'y' or 'n'.\n");
         }
      }
      // Consume newline and any extra characters
      while (getchar() != '\n');
   } while (!valid_choice);

   HashTable* table = NULL;

   if (use_persistence) {
      // Try to load from AOF
      table = aof_load(FILENAME);
      if (table) {
         printf("Database loaded from AOF file.\n");
         // Re-open for appending
         if (!aof_init(FILENAME)) {
             printf("Warning: Could not open AOF file for writing.\n");
         }
      } else {
         printf("No existing AOF file found or load failed.\n");
      }
   }

   // If table is still NULL (no persistence or load failed), create new
   if (table == NULL) {
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

      // If persistence is enabled, we need to initialize the file
      // and write the table size as header (if we decide to support size header)
      if (use_persistence) {
          // Create the new AOF file with the size header
          if (aof_create_empty(FILENAME, table->size)) {
              // Now open in append mode
              aof_init(FILENAME);
              printf("Persistence enabled. AOF file initialized.\n");
          } else {
              printf("Error creating AOF file.\n");
          }
      }
   }

   // Start the loop
   server_run(table);
   
   // Cleanup
   if (use_persistence) {
       aof_close();
   }
   ht_destroy(table);
   return 0;
}