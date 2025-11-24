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
char** parse_cmd (char* cmd) {
   // Using strtok to split the string into 3
   // (or 2 in case of a "GET" command) tokens
   char** args;
   // Get the specific command
   args[0] = strtok(cmd, " ");
   // Get the key
   args[1] = strtok(NULL, " ");
   // Get the value
   args[2] = strtok(NULL, " ");

   return args;
}

void execute_cmd (HashTable* table, char** args) {
   // DISPATCHER:
   if (strcmp(args[0], "SET") == 0) {
      // If the first arg is "SET", the dispatcher
      // has to execute the ht_insert function
      if (args[1] != NULL && args[2] != NULL) {
         if (ht_insert(table, args[1], args[2])) {
            // If the insert operation returns true...
            printf("\nDone! Data (key: %s, value: %s) was inserted succesfully.", args[1], args[2]);
         } else {
            // Else return the error message
            printf("\nERR 2: insertion failed!");
         }
      }
   } else if (strcmp(args[0], "GET")) {
      // If the first arg is "GET", the dispatcher
      // has to execute the ht_get function
      char* searched_value = ht_get(table, args[1]);
      // Check if the key exists
      if (args[1] != NULL) {
         if (searched_value) {
            // If the get operation returns something...
            printf("\nDone! The value corresponding to this key %s is: %s", args[1], searched_value);
         } else {
            // Otherwise return the message
            printf("\nThe key was not found.");
         }
      }
   } else if (strcmp(args[0], "DEL")) {
      // TODO: DEL command not yet supported      
   } else if (strcmp(args[0], "EXIT")) {
      // If the first arg is "EXIT", close and exit the program
      printf("\nThanks for using mini-redis!");
      return;
   } else {
      printf("\nERR 1: command is not valid, '%s'", args[0]);
   }
}

// Start server function
void server_run (HashTable* table) {
   // Static buffer on the stack
   char buffer[MAX_CMD_SIZE];

   while (1) {
      printf("mini-redis> ");

      // User insterts the command
      // Reading the string from standard input stream
      fgets(buffer, MAX_CMD_SIZE, stdin);
      if (!buffer) {
         printf("\nERR 0: error during memory allocation.");
         EXIT(0);
      }

      // Trimming '\n' character
      trim_newline(buffer);
      // Parsing the command in tokens
      char** args = parse_cmd(buffer);
      // Dispatcher that executes the correct function
      execute_cmd(args);
   }
}