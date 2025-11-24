#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/server.h"

#define MAX_CMD_SIZE 1024

// First function
void trim_newline (char* cmd) {
   // Get the lenght of the command inserted
   // Calling strlen only one time (time complexity O(N))
   size_t len = strlen(cmd);

   // Check if the last character is the new line character
   if (len > 0 && cmd[len - 1] == '\n') {
      // If it is, replace it with \0
      cmd[len - 1] = '\0';
   }
}

// Second function
char** parse_and_execute (char* cmd) {

}

// Start server function
void server_run (HashTable* table) {
   // Static buffer on the stack
   char buffer[MAX_CMD_SIZE];

   // ...
}