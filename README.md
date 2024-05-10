# ft_irc


## Description:

ft_irc is an IRC (Internet Relay Chat) server implementation written in C++98. It provides a platform for real-time text-based communication over TCP/IP between multiple clients. The server supports essential IRC functionalities such as user authentication, channel management, private messaging, and operator privileges.
## Project Structure

The ft_irc project is organized into several directories, each serving a specific purpose. Here's an overview:

- **src/:** Contains the source code files for the ft_irc server.
- **inc/:** Contains header files necessary for compiling the project.
- **Makefile:** Makefile for compiling and managing the project.

## Features

- **Multi-Client Support:** The server can handle multiple clients simultaneously without blocking.
- **Non-Blocking I/O:** All I/O operations are non-blocking to ensure smooth server performance.
- **TCP/IP Communication:** Communication between the server and clients is over TCP/IP (v4 or v6).
- **User Authentication:** Clients must authenticate using a connection password to access the server.
- **Channel Operations:** Supports channel creation, joining, leaving, and messaging.
- **Private Messaging:** Clients can send and receive private messages to/from other users.
- **Operator Privileges:** Users with operator privileges can perform special commands like KICK, INVITE, TOPIC, and MODE.
- **Clean Code:** The codebase follows best practices for readability and maintainability.

## Compilation:

To compile the project, simply run 'make':

```bash
make
```
## Usage 
Run the compiled program to start the ft_irc server:

```bash
./ircserv <port> <password>
```
- **port:** The port number on which the IRC server listens for incoming connections.
- **password:** The connection password required by clients to connect to the server.


After setting up the server, it's ready to accept connections. To connect to it, you can utilize various IRC clients or use the nc (NetCat) utility. Below is an example of how to connect using nc(do this in a different terminal):
```bash
nc localhost <port_number>
```
Replace <port_number> with the port number on which the server is listening. Once connected, you have to authenticate your client with a password, nickname, and username, and then you can start interacting with the server through the IRC protocol.

## Bonus Features
Enhanced the IRC server with additional features:

File Transfer: Allowed clients to transfer files between each other.

Bot: Its purpose is to connect to an IRC server, join a specified channel, and listen for messages in that channel. When certain keywords, such as "bitcoin", are detected in the channel messages, the bot responds with a predefined message.
## Collaboration:

This three-person project was done with [OcelotHero](https://github.com/OcelotHero) & [krishirajsinh-p](https://github.com/krishirajsinh-p).

## 🚀 Next Project

[Inception](https://github.com/adhaka-afk/Inception)

## ⏳ Previous Project

[cub3D](https://github.com/adhaka-afk/cub3D)
