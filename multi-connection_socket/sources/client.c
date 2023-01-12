#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include "inet_sockets.h"

#define BUF_SIZE 4096

int communicate(int cfd)
{
    while (1)
    {
        printf("%% ");
        char buf[BUF_SIZE];
        scanf("%s", buf);
        
        int numSend = send(cfd, buf, strlen(buf), 0);
        if (numSend == -1)
        {
            perror("send()");
            return -1;
        }

        int numRecv = recv(cfd, buf, BUF_SIZE, 0);
        if (numRecv == -1)
        {
            perror("recv()");
            return -1;
        }
        printf("%s\n", buf);
    }
    return 0;
}

int startClient(const char *ip, const int port)
{
    printf("Starting client...\n");
    int cfd = inetConnect(ip, port, SOCK_STREAM);
    if (cfd == -1)
    {
        perror("inetConnect()");
        return -1;
    }

    return communicate(cfd);
}

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        perror("Invalid command!");
        return -1;
    }

    int port = atoi(argv[2]);

    return startClient(argv[1], port);
}