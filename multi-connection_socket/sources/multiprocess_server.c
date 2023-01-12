#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "inet_sockets.h"

#define NUM_CONNECTION 10
#define BUF_SIZE 4096

int handleRequest(int cfd)
{
    printf("Handling request...\n");

    char buf[BUF_SIZE];
    ssize_t numRecv;
    while ((numRecv = recv(cfd, buf, BUF_SIZE, 0)) > 0)
    {
        printf("%s\n", buf);
        if (send(cfd, buf, numRecv, 0) != numRecv)
        {
            perror("send()");
            return 0;
        }
    }
    if (numRecv == -1)
    {
        perror("recv()");
        return -1;
    }
}

int startServer(const int port)
{
    printf("Starting server...\n");

    int lfd = inetListen(port, NUM_CONNECTION);
    if (lfd == -1)
    {
        perror("listen()");
        return -1;
    }

    while (1)
    {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1)
        {
            perror("accept()");
            return -1;
        }

        printf("Creating new connection...\n");
        switch (fork())
        {
            case -1:
            {
                perror("fork()");
                close(cfd);
                break;
            }
            case 0:
            {
                close(lfd);
                handleRequest(cfd);
                return 0;
            }
            default:
            {
                close(cfd);
                break;
            }
        }
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid command!\n");
        return -1;
    }

    return startServer(atoi(argv[1]));
}