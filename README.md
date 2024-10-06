# 42_FTIRC
Internet Relay Chat in C++

## Project Overview

This project implements a custom **IRC (Internet Relay Chat) server** in C++, handling multiple clients simultaneously. The server supports basic IRC commands, such as joining channels, sending messages, and handling client disconnections. It also comes with bot functionality and leverages network programming techniques to create a scalable, efficient, and feature-rich server.

## Features

- **Multi-client support**: Handles multiple client connections concurrently using `select()` for monitoring file descriptors.
- **Command parsing**: Supports parsing user commands like `/join`, `/msg`, and `/quit`, and processes them appropriately.
- **Bot functionality**: A built-in bot that responds to specific commands and even detects pangrams!
- **Real-time messaging**: Facilitates real-time message broadcasting between clients in different channels.
- **Fault-tolerance**: Handles client crashes and unexpected disconnections gracefully.
- **Customizable Responses**: Use of macros to define and send IRC response messages (e.g., welcome, your host).

## Technologies

- **C++98**: The project is written using the C++98 standard.
- **POSIX Sockets**: Network communication is handled using POSIX sockets.
- **File Descriptor Multiplexing**: `select()` is used to manage multiple socket connections in a non-blocking manner.
- **Memory Management**: Careful memory management ensures that client objects are correctly created and destroyed.

## Key Components

### 1. **Connection Handling**
The server uses a non-blocking model to handle new client connections and monitor existing ones. The server listens for incoming connections, accepts them, and creates a `Client` object to manage each connection.

### 2. **Command Parsing**
Clients send messages to the server, which parses the buffer and processes commands. The `parse_buffer()` function ensures that incoming data is handled efficiently, breaking down messages into discrete commands.

### 3. **Bot Interaction**
The server includes a bot that interacts with users and detects pangrams in messages. When enabled, the bot monitors chat messages and informs users if a message is a pangram.

### 4. **Error Handling and Cleanup**
The server handles errors and unexpected client disconnections, ensuring that file descriptors are cleaned up and no memory leaks occur.

## Usage

### Prerequisites
- A C++98 compliant compiler (e.g., g++)
- A UNIX-based system (e.g., Linux, macOS) for socket programming support

### Compilation
- After compiling using `make`, you can start the server by running:

` ./ircserv <port> <password> `

## Authors

- [Pedro Mota](https://github.com/peterbikes/) 
- [Helder Cabo](https://github.com/EcoGecko/)