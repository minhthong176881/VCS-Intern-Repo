#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/queue.h>
#include <string.h>
#include "inet_socket.h"

#define BACKLOGS 1000

struct client_t {
    int fd;
    char *name;
};

struct client_t clients[BACKLOGS];
int num_clients = 0;

int client_list_init()
{
    for (int i = 0; i < BACKLOGS; i++)
    {
        clients[i].fd = -1;
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
        if (clients[i].fd == -1)
        {
            clients[i].fd = cfd;
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
        if (clients[i].fd == cfd)
        {
            clients[i].fd == -1;
            return 0;
        }
    }

    return 0;
}

int send_to_all(char *buf)
{
    for (int i = 0; i < BACKLOGS; i++)
    {
        if (clients[i].fd != -1)
        {
            if (send(clients[i].fd, buf, strlen(buf) + 1, 0) < 0)
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
    
    // Get ID
    if (recv(cfd, buf, BUFSIZ, 0) < 0)
    {
        perror("recv()");
        return (void *)-1;
    }

    // Bind name to cfd
    char *name = malloc(strlen(buf) + 1);
    strncpy(name, buf, strlen(buf));
    for (int i = 0; i < BACKLOGS; i++)
    {
        if (cfd == clients[i].fd)
        {
            clients[i].name = malloc(strlen(name) + 1);
            strncpy(clients[i].name, name, strlen(name));
            break;
        }
    }
    
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
            sprintf(buf, "%s closed\n", name);
            printf("%s\n", buf);
            delete_clients(cfd);
            send_to_all(buf);
            break;
        }

        printf("Client %d: %s\n", cfd, buf);

        char *tmp = malloc(strlen(buf) + 1);
        strncpy(tmp, buf, strlen(buf));
        sprintf(buf, "%s: %s", name, tmp);
        send_to_all(buf);
    }

    free(name);

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
        
        struct client_t client = {cfd, NULL};
        void *arg = malloc(sizeof(struct client_t));
        memcpy(arg, &client, sizeof(struct client_t));

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