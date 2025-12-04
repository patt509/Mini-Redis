#include <stdio.h>
#include <stdlib.h>
#include "./include/hashtable.h"
#include "./include/server.h"

int main() {
   int size_input;
   printf("Enter hash table size: ");
   if (scanf("%d", &size_input) != 1) {
      printf("Invalid input.\n");
      return 1;
   }

   // Consume newline left by scanf
   while (getchar() != '\n');

   HashTable* table = ht_load("dump.bin");
   if (table == NULL) {
      printf("Error during table loading from file.\n");
   }

   // HashTable* table = ht_create(size_input);
   // if (!table) {
   //    printf("Error during table allocation.");

   //    return 1;
   // }

   // Start the loop
   server_run(table);
}