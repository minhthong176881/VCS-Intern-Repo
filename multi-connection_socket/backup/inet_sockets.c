#define _DEFAULT_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int inetConnect(const char *host, const char *service, int type)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;

    struct addrinfo *result;
    if (getaddrinfo(host, service, &hints, &result) != 0)
    {
        perror("getaddrinfo()");
        return -1;
    }

    struct addrinfo *rp;
    int sfd;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
        {
            continue;
        }

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
        {
            break;
        }

        close(sfd);
    }

    freeaddrinfo(result);

    return (rp == NULL) ? -1 : sfd;
}

static int inetPassiveSocket(const char *service, int type, socklen_t *addrlen, bool doListen, int backlog)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *result;
    if (getaddrinfo(NULL, service, &hints, &result) != 0)
    {
        perror("getaddrinfo()");
        return -1;
    }

    struct addrinfo *rp;
    int sfd;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
        {
            continue;
        }

        int optval = 1;
        if (doListen)
        {
            if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            {
                close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        }

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            break;
        }

        close(sfd);
    }

    if (rp != NULL && doListen)
    {
        if (listen(sfd, backlog) == -1)
        {
            freeaddrinfo(result);
            return -1;
        }
    }

    if (rp != NULL && addrlen != NULL)
    {
        *addrlen = rp->ai_addrlen;
    }

    freeaddrinfo(result);

    return (rp == NULL) ? -1 : sfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen)
{
    return inetPassiveSocket(service, SOCK_STREAM, addrlen, true, backlog);
}

int inetBind(const char *service, int type, socklen_t *addrlen)
{
    return inetPassiveSocket(service, type, addrlen, false, 0);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrlen)
{
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) == 0)
    {
        snprintf(addrStr, addrStrlen, "(%s, %s)", host, service);
    }
    else
    {
        snprintf(addrStr, addrStrlen, "(?UNKNOWN?)");
    }

    return addrStr;
}