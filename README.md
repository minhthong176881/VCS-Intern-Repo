# Dev core network assignment

## Internet Domain Socket Library

The Internet Socket Library implements the functions to perform tasks commonly required for Internet domain sockets. These function is defined in inet_socket.h header file.

``` C
int inet_connect(char *ip, int port, int type);
```

The inet_connect() function creates a socket with the given socket type, and connects it to the address specified by IPv4 address and port number. This function is designed for TCP or UDP clients that need to connect their socket to a server socket.

``` C
int inet_bind(int port, int type);
```

The inet_bind() function creates a socket of the given type, bound to the wildcard
IP address on the port specified by port number and type. (The socket type indicates whether this is a TCP or UDP service.) This function is designed (primarily) for UDP servers and clients to create a socket bound to a specific address.

``` C
int inet_listen(int port, int backlog);
```

The inet_listen() function creates a listening stream (SOCK_STREAM) socket bound to the wildcard IP address on the TCP port specified by service. This function is designed for use by TCP servers.

## A server that can handle up to 1000 connections

### How to implement

1. Create a TCP socket
2. Accept incoming connection
3. With each connection, create a thread to handle the connection
4. In the handling connection function, when receiving any message, send it back to the client

## Chat Group

This assignment require to implement a server that can receive a message from a client and then send it to all clients that connected to the server.

### How to implement

First, create a structure to store required information.

``` C
struct client_t {
    int fd;
    char *name;
};
```

Then, create an array to store up to 1000 clients information.

After that, follow these steps:

1. Create a TCP socket.
2. Accept incoming connections
3. With each connection:
   * Add `fd` to client infomation array.
   * Create a thread to handle connection
4. In the handling connection function:
   * First, get the user identity that is received from the client.
   * Add the name to the client infomation array
   * When receiving any message, make a loop through client information array, send to all clients in the array.
