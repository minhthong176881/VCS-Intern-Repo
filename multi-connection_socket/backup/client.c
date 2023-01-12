#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "inet_sockets.h"

#define BUF_SIZE 4096

int communicate(int cfd)
{
    while (1)
    {
        printf("%% ");
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        scanf("%s", buffer);
        
        if (send(cfd, buffer, strlen(buffer) + 1, 0) == -1)
        {
            perror("send()");
        }

        if (recv(cfd, buffer, BUF_SIZE, 0) == -1)
        {
            perror("recv()");
        }

        printf("%s\n", buffer);
    }

    return 0;
}

int startClient(const char *host, const char* service, int type)
{
    int cfd = inetConnect(host, service, type);
    if (cfd == -1)
    {
        perror("inetConnect()");
        return -1;
    }

    communicate(cfd);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid command!");
    }

    return startClient(argv[1], argv[2], SOCK_STREAM);
}