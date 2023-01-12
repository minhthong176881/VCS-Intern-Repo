#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdbool.h>
#include "inet_sockets.h"

#define NUM_CONNECTION 1000
#define BUF_SIZE 4096

struct Param
{
    int cfd;
    bool *working;
};

void *handleConnection(void *arg)
{
    printf("Handling request...\n");
    
    struct Param carg = *(struct Param *)arg;
    int cfd = carg.cfd;
    bool working = *carg.working;

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
        return (void *)-1;
    }
}

int startServer(int port)
{
    printf("Starting server...\n");

    int lfd = inetListen(port, NUM_CONNECTION);
    if (lfd == -1)
    {
        perror("inetListen()\n");
        return -1;
    }

    pthread_t threads[NUM_CONNECTION];
    bool working[NUM_CONNECTION];
    memset(&working, 0, sizeof(int) * NUM_CONNECTION);

    while (1)
    {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1)
        {
            perror("accept()");
            return -1;
        }

        for (int i = 0; i < NUM_CONNECTION; i++)
        {
            if (!working[i])
            {
                working[i] = true;
                struct Param parg;
                parg.cfd = cfd;
                parg.working = &working[i];
                pthread_create(&threads[i], NULL, handleConnection, (void *)&parg);
            }
        }
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid command!");
        return -1;
    }

    return startServer(atoi(argv[1]));
}