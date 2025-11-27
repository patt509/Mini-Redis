#include <stdio.h>
#include <stdlib.h>
#include "./include/hashtable.h"
#include "./include/server.h"

int next_pow2(int n) {
   if (n < 1) return 1;
   int p = 1;
   while (p < n) {
      p <<= 1;
   }
   return p;
}

int main() {
   int size_input;
   printf("Enter hash table size: ");
   if (scanf("%d", &size_input) != 1) {
      printf("Invalid input.\n");
      return 1;
   }

   // Consume newline left by scanf
   while (getchar() != '\n');

   int size = next_pow2(size_input);
   printf("Initializing hash table with size: %d\n", size);

   HashTable* table = ht_create(size);
   if (!table) {
      printf("Error during table allocation.");

      return 1;
   }

   // Start the loop
   server_run(table);
}