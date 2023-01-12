#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include "inet_sockets.h"

#define SERVICE "echo"
#define BUF_SIZE 4096
#define NUM_CONNECTION 2000

int handleRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
    {
        if (write(cfd, buf, numRead) != numRead)
        {
            perror("write()");
            return -1;
        }

        if (numRead == -1)
        {
            return -1;
        }
    }
}

int startServer()
{
    int lfd = inetListen(SERVICE, NUM_CONNECTION, NULL);
    if (lfd == -1)
    {
        perror("inetListen()");
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
                close(cfd);
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

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Invalid command!");
        return -1;
    }

    return startServer();
}