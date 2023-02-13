#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "inet_socket.h"

void *receive_message(void *arg)
{
    int cfd = *(int *)arg;
    
    char buf[BUFSIZ];
    while (1)
    {
        int numRead = recv(cfd, buf, BUFSIZ, 0);
        if (numRead < 0)
        {
            perror("recv()");
            return (void *)-1;
        }
        if (numRead == 0)
        {
            printf("Server closed\n");
            return (void *)-1;
        }
        printf("%s\n", buf);
    }
}

int communicate(int cfd)
{
    char buf[BUFSIZ];
    
    // Get ID
    printf("Enter user name: ");
    scanf("%[^\n]", buf);
    getchar();
    if (send(cfd, buf, strlen(buf) + 1, 0) < 0)
    {
        perror("send()");
        return -1;
    }

    while (1)
    {
        scanf("%[^\n]", buf);
        getchar();
        if (send(cfd, buf, strlen(buf) + 1, 0) < 0)
        {
            perror("send()");
            return -1;
        }
    }
}

int start_client(char *ip, int port)
{
    int cfd = inet_connect(ip, port, SOCK_STREAM);
    if (cfd == -1)
    {
        return -1;
    }

    void *arg = malloc(sizeof(int));
    *(int *)arg = cfd;

    pthread_t thread;
    pthread_create(&thread, NULL, receive_message, arg);

    communicate(cfd);

    free(arg);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid command!\n");
        return 1;
    }

    int port = atoi(argv[2]);
    if (port == 0)
    {
        printf("Invalid port number!\n");
        return 1;
    }

    return start_client(argv[1], port);
}