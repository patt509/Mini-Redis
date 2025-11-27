#include <stdio.h>
#include "./include/hashtable.h"
#include "./include/server.h"

int main() {
   HashTable* table = ht_create();
   if (!table) {
      printf("Error during table allocation.");

      return 1;
   }

   // Start the loop
   server_run(table);
}