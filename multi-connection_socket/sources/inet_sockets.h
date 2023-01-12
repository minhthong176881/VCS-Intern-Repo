#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H

int inetConnect(const char *ip, const int port, const int type);

int inetListen(const int port, const int backlog);

int inetBind(const char *ip, const int port);

#endif