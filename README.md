# Mini-Redis

A lightweight, in-memory key-value store implementation in C, inspired by Redis. This project demonstrates low-level system programming concepts including TCP socket networking, manual memory management, custom data structures, and file-based persistence.

## Overview

Mini-Redis functions as a TCP server daemon that listens for incoming connections and processes text-based commands. It implements a robust Hash Table for data storage and an Append-Only File (AOF) mechanism to ensure data durability across restarts. The codebase is designed with modularity and strict memory management in mind, verified to be free of memory leaks using Valgrind.

## Key Features

- **TCP Server Architecture**: Implements a custom TCP server using POSIX sockets, handling client connections via standard tools like `netcat` or `telnet`.
- **Efficient Data Storage**: Uses a dynamically allocated Hash Table with chaining for collision resolution and the FNV-1a hashing algorithm for uniform distribution.
- **AOF Persistence**: Implements an Append-Only File strategy to log write operations in real-time, allowing full state reconstruction upon server startup.
- **Memory Safety**: Rigorous manual memory management with zero leaks.
- **Interactive Protocol**: Features a text-based communication protocol with ANSI color support for improved readability in terminal clients.

## Supported Commands

The server accepts the following commands:

- `SET <key> <value>`: Stores a key-value pair in the database.
- `GET <key>`: Retrieves the value associated with a specific key.
- `DEL <key>`: Removes a key and its associated value from the database.
- `EXIT`: Terminates the client connection.

## Getting Started

### Prerequisites

- GCC Compiler
- Make (optional)
- Linux/Unix environment (for POSIX socket support)

### Compilation

To compile the project, use the following command:

```bash
gcc -o mini-redis main.c src/server.c src/hashtable.c src/persistence.c -I./include
```

### Usage

1. **Start the Server**:
   Run the executable. You will be prompted to enable persistence and set the initial table size if no existing data is found.

   ```bash
   ./mini-redis
   ```

2. **Connect a Client**:
   Open a separate terminal window and connect using `netcat` (or `telnet`):

   ```bash
   nc localhost 8080
   ```

## Persistence

Mini-Redis uses an Append-Only File (`dump.aof`) to ensure data durability.
- **Write Operations**: Every `SET` and `DEL` operation is immediately appended to the AOF log in a binary format.
- **Recovery**: On startup, the server reads the AOF log to replay all operations and reconstruct the in-memory state.

## Project Structure

- `src/`: Source files for the Hash Table, Server logic, and Persistence module.
- `include/`: Header files defining interfaces and data structures.
- `main.c`: Entry point handling initialization and configuration.

## Roadmap

- [ ] **Concurrency**: Implement multi-threading (using pthreads) to handle multiple clients simultaneously.
- [ ] **Protocol Improvements**: Support for quoted strings and spaces in values.
- [ ] **Advanced Commands**: Implementation of `KEYS`, `FLUSHALL`, and expiration times.

