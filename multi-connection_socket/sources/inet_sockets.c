#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>

int inetConnect(const char *ip, const int port, const int type)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    memset(&addr, 0, addrlen);
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int sockfd = socket(addr.sin_family, type, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        return -1;
    }

    if (connect(sockfd, (const struct sockaddr *)&addr, addrlen) == -1)
    {
        perror("connect()");
        return -1;
    }

    return sockfd;
}

int inetListen(const int port, const int backlog)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    memset(&addr, 0, addrlen);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int lfd = socket(addr.sin_family, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket()");
        return -1;
    }

    if (bind(lfd, (const struct sockaddr *)&addr, addrlen) == -1)
    {
        perror("bind()");
        return -1;
    }

    if (listen(lfd, backlog) == -1)
    {
        perror("listen()");
        return -1;
    }

    return lfd;
}

int inetBind(const int port, const int type)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    memset(&addr, 0, addrlen);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int sockfd = socket(addr.sin_family, type, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        return -1;
    }

    if (bind(sockfd, (const struct sockaddr *)&addr, addrlen) == -1)
    {
        perror("bind()");
        return -1;
    }

    return sockfd;
}