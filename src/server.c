#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/server.h"
#include "../include/persistence.h"

#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons, inet_ntoa
#include <unistd.h>     // read, write, close

#define PORT 8080
#define MAX_CMD_SIZE 1024

// ANSI C codes to enhance terminal output
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define LIGHT_BLUE  "\033[1;34m"
#define RESET       "\033[0m"

void trim_newline(char* cmd, int len) {
   // Add \0 at the end of the string
   cmd[len] = '\0';

   // Remove trailing \n (if present)
   if (len > 0 && cmd[len - 1] == '\n') {
      cmd[len - 1] = '\0';
      len--;
   }
   // Remove trailing \r (handles \r\n from Windows/Telnet)
   if (len > 0 && cmd[len - 1] == '\r') {
      cmd[len - 1] = '\0';
   }
}

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
bool execute_cmd (HashTable* table, char** args, int client_fd) {
   if (args == NULL || args[0] == NULL) {
      return true; // Empty command, continue loop
   }

   // DISPATCHER:
   if (strcmp(args[0], "SET") == 0) {
      // If the first arg is "SET", the dispatcher
      // has to execute the ht_insert function
      if (args[1] == NULL || args[2] == NULL) {
         dprintf(client_fd, "%sERR 2%s: SET requires two parameters: key and value\n", RED, RESET);
         return true;
      }

      if (ht_insert(table, args[1], args[2])) {
         // If the insert operation returns true...
         dprintf(client_fd, "Data (key: %s%s%s, value: %s%s%s) inserted succesfully!\n", GREEN, args[1], RESET, GREEN, args[2], RESET);
         
         // Log to AOF
         aof_log_set(args[1], args[2]);
      } else {
         // Else return the error message
         dprintf(client_fd, "%sERR 3%s: insertion failed!\n", RED, RESET);
      }

      return true;
   } else if (strcmp(args[0], "GET") == 0) {
      // If the first arg is "GET", the dispatcher
      // has to execute the ht_get function
      if (args[2] != NULL) {
         dprintf(client_fd, "%sERR 4%s: too many arguments, GET command only accepts one parameter, the key.\n", RED, RESET);
         return true;
      }

      if (args[1] == NULL) {
         dprintf(client_fd, "%sERR 5%s: GET needs one argument, the key.\n", RED, RESET);
         return true;
      }

      char* searched_value = ht_get(table, args[1]);
      if (searched_value) {
         // If the get operation returns something...
         dprintf(client_fd, "The value corresponding to this key %s%s%s is: %s%s%s\n", GREEN, args[1], RESET, GREEN, searched_value, RESET);
      } else {
         // Otherwise return the message
         dprintf(client_fd, "The key was not found.\n");
      }

      return true;
   } else if (strcmp(args[0], "DEL") == 0) {
      if (args[2] != NULL) {
         dprintf(client_fd, "%sERR 4%s: too many arguments, DEL command only accepts one parameter, the key.\n", RED, RESET);
         return true;
      }

      if (args[1] == NULL) {
         dprintf(client_fd, "%sERR 5%s: DEL needs one argument, the key.\n", RED, RESET);
         return true;
      }

      bool success = ht_delete(table, args[1]);
      if (success) {
         dprintf(client_fd, "The element was deleted successfully!\n");
         
         // Log to AOF
         aof_log_del(args[1]);
      } else {
         /*
            Currently, this error can be seen for only two different reasons:
            1) The table does not exixst.
            2) The nod containing the key was not found.

            Since main.c gurantees us that the table exists, the only
            possible way to enter this branch of the if-else
            construct is by entering a key wich is not in the table.

            TODO: enum with error codes for more specific error description.
         */
         dprintf(client_fd, "%sERR 5%s: the key was not found.\n", RED, RESET);
      }

      return true;
   } else if (strcmp(args[0], "EXIT") == 0) {
      // If the first arg is "EXIT", close and exit the program
      dprintf(client_fd, "Thanks for using mini-redis!\n");

      return false;
   } else {
      // In case the command is a not handled case
      dprintf(client_fd, "ERR 1: command is not valid, '%s'\n", args[0]);

      return true;
   }
}

// Start server function
void server_run (HashTable* table) {
   struct sockaddr_in address;
   address.sin_family = AF_INET;
   address.sin_port = htons(PORT); // Port numebr normalization
   address.sin_addr.s_addr = INADDR_ANY; // Listen every network card

   // Server file descriptor (0 = protocol is chosen automatically)
   int server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (server_fd < 0) {
      perror("Error: failed to start the server.\n");
      return;
   }

   // Binding the socket with the designated port
   bind(server_fd, (struct sockaddr*)&address, sizeof(struct sockaddr_in));

   // OS needs to be aware that messages could come through the socket
   listen(server_fd, 4); // Max 4 connections at a time

   // accept() creates the client socket
   struct sockaddr_in client_addr;
   socklen_t client_len = sizeof(client_addr);
   int client_fd;
   // Server cycle
   while (1) {
      printf("Waiting for connections...\n");
      client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
      if (client_fd < 0) {
         perror("Accept failed!\n");
         continue;
      }
      printf("New client connected!\n");
      printf("Client IP: %s\n", inet_ntoa(client_addr.sin_addr));
      printf("Port: %d\n", ntohs(client_addr.sin_port));

      dprintf(client_fd, "Welcome to %smini-redis%s!\n", GREEN, RESET);
      // Static buffer on the stack
      char buffer[MAX_CMD_SIZE];
      char* args[3]; // Array of pointers on the stack
      // Conversation loop
      while (1) {
         dprintf(client_fd, "%smini-redis>%s ", BLUE, RESET);
         // User insterts the command
         // Reading the string from the client socket
         // P.S. -1 because read() does not automatically put \0
         // at the end of the string
         int result = read(client_fd, buffer, MAX_CMD_SIZE - 1);
         if (result == 0) {
            printf("Connection closed.\n");
            dprintf(client_fd, "Thanks for using mini-redis!\n");
            close(client_fd);
            break;
         } else if (result < 0) {
            perror("Error reading the command.\n");
            continue;
         }

         // Set the '\0' character and trim \n or \r at the end
         trim_newline(buffer, result);

         // Parsing the command in tokens
         parse_cmd(buffer, args);

         /*
            Dispatcher that execute the correct function and
            returns a boolean value:
            - if it returns true, the loop continues
            - if it returns false, it means the use run the EXIT command
            so the cycle needs to stop
         */
         bool continue_loop = execute_cmd(table, args, client_fd);
         
         if (!continue_loop) {
            break;
         }
      }

      printf("Client %s:%d disconnected.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      close(client_fd);
   }
}