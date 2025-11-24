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
void parse_cmd (char* cmd, char* args[]) {
   // Using strtok to split the string into 3
   // (or 2 in case of a "GET" command) tokens
   
   // Get the specific command
   args[0] = strtok(cmd, " ");
   // Get the key
   args[1] = strtok(NULL, " ");
   // Get the value
   args[2] = strtok(NULL, " ");
}

// Returns bool false in case of EXIT command
bool execute_cmd (HashTable* table, char** args) {
   if (args == NULL || args[0] == NULL) {
      return true; // Empty command, continue loop
   }

   // DISPATCHER:
   if (strcmp(args[0], "SET") == 0) {
      // If the first arg is "SET", the dispatcher
      // has to execute the ht_insert function
      if (args[1] != NULL && args[2] != NULL) {
         if (ht_insert(table, args[1], args[2])) {
            // If the insert operation returns true...
            printf("Done! Data (key: %s, value: %s) was inserted succesfully.\n", args[1], args[2]);
         } else {
            // Else return the error message
            printf("ERR 2: insertion failed!\n");
         }
      } else {
         printf("ERR: SET requires key and value\n");
      }

      return true;
   } else if (strcmp(args[0], "GET") == 0) {
      // If the first arg is "GET", the dispatcher
      // has to execute the ht_get function
      if (args[1] != NULL) {
         char* searched_value = ht_get(table, args[1]);
         if (searched_value) {
            // If the get operation returns something...
            printf("Done! The value corresponding to this key %s is: %s\n", args[1], searched_value);
         } else {
            // Otherwise return the message
            printf("The key was not found.\n");
         }
      } else {
         printf("ERR: GET requires a key\n");
      }

      return true;
   } else if (strcmp(args[0], "DEL") == 0) {
      // TODO: DEL command not yet supported
      printf("DEL command not yet supported\n");
      return true;      
   } else if (strcmp(args[0], "EXIT") == 0) {
      // If the first arg is "EXIT", close and exit the program
      printf("Thanks for using mini-redis!\n");

      return false;
   } else {
      printf("ERR 1: command is not valid, '%s'\n", args[0]);

      return true;
   }
}

// Start server function
void server_run (HashTable* table) {
   // Static buffer on the stack
   char buffer[MAX_CMD_SIZE];
   char* args[3]; // Array of pointers on the stack

   while (1) {
      printf("mini-redis> ");

      // User insterts the command
      // Reading the string from standard input stream
      if (fgets(buffer, MAX_CMD_SIZE, stdin) == NULL) {
         break; // Handle EOF (Ctrl+D)
      }

      // Trimming '\n' character
      trim_newline(buffer);
      // Parsing the command in tokens
      parse_cmd(buffer, args);

      /*
         Dispatcher that execute the correct function and
         returns a boolean value:
         - if it returns true, the loop continues
         - if it returns false, it means the use run the EXIT command
           so the cycle needs to stop
      */
      bool continue_loop = execute_cmd(table, args);
      
      if (!continue_loop) {
         break;
      }
   }
}