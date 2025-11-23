#ifndef SERVER_H
#define SERVER_H

#include "./hashtable.h"

// Only public function main.c will see and call
// to start the server
void server_run (HashTable* table);

#endif