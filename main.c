#include <stdio.h>
#include "./include/hashtable.h"
#include "./include/server.h"

#define HT_SIZE 1024

int main() {
   HashTable* table = ht_create(HT_SIZE);
   if (!table) {
      printf("Error during table allocation.");

      return 1;
   }

   // Start the loop
   server_run(table);
}