#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/queue.h>
#include <string.h>
#include "inet_socket.h"

#define BACKLOGS 1000

int clients[BACKLOGS] = {-1};
int num_clients = 0;

int client_list_init()
{
    for (int i = 0; i < BACKLOGS; i++)
    {
        clients[i] = -1;
    }
    return 0;
}

int add_client(int cfd)
{
    if (num_clients == BACKLOGS)
    {
        printf("Full\n");
        return 0;
    }

    for (int i = 0; i < BACKLOGS; i++)
    {
        if (clients[i] == -1)
        {
            clients[i] = cfd;
            ++num_clients;
            return 0;
        }
    }

    return 0;
}

int delete_clients(int cfd)
{
    for (int i = 0; i < BACKLOGS; i++)
    {
        if (clients[i] == cfd)
        {
            clients[i] == -1;
            return 0;
        }
    }

    return 0;
}

int send_to_all(char *buf)
{
    for (int i = 0; i < BACKLOGS; i++)
    {
        if (clients[i] != -1)
        {
            if (send(clients[i], buf, strlen(buf) + 1, 0) < 0)
            {
                perror("send()");
                return -1;
            }
        }
    }
}

void *handle_connection(void *arg)
{
    int cfd = *(int *)arg;
    printf("Client %d connected!\n", cfd);

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
            sprintf(buf, "Client %d closed\n", cfd);
            printf("%s\n", buf);
            send_to_all(buf);
            break;
        }

        printf("Client %d: %s\n", cfd, buf);

        send_to_all(buf);
    }

    return 0;
}

int start_server(int port)
{
    int sfd = inet_listen(port, BACKLOGS);
    if (sfd == -1)
    {
        return -1;
    }

    while (1)
    {
        int cfd = accept(sfd, NULL, NULL);
        if (cfd == -1)
        {
            perror("accept()");
            return cfd;
        }
        
        add_client(cfd);

        void *arg = malloc(sizeof(int));
        *(int *)arg = cfd;
        
        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, arg);
    }
}

int main(int argc, char *argv[])
{
    client_list_init();

    if (argc != 2)
    {
        printf("Invalid command!\n");
        return 1;
    }

    int port = atoi(argv[1]);
    if (port == 0)
    {
        printf("Invalid port number!\n");
        return 1;
    }

    return start_server(port);
}