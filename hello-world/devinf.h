#pragma once

#include <sys/types.h>
#include <sys/socket.h>

const int SOCKET_ERROR = -1;
const int INVALID_SOCKET = -1;

class devinf
{
public:
    virtual int getaddrinfoI(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) = 0;
    virtual void freeaddrinfoI(struct addrinfo *res) = 0;
    virtual int socketI(int domain, int type, int protocol) = 0;
    virtual int setsocketblock(int s, bool block) = 0;
    virtual int bindI(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
    virtual int listenI(int sockfd, int backlog) = 0;
    virtual int selectI(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) = 0;
    virtual int acceptI(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;
    virtual int connectI(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
    virtual int recvI(int sockfd, void *buf, size_t len, int flags) = 0;
    virtual int sendI(int sockfd, const void *buf, size_t len, int flags) = 0;
    virtual int shutdownI(int sockfd, int how) = 0;
    virtual int closesocketI(int fd) = 0;
    virtual int setsocketreuseaddr(int sockfd) = 0 ;
};

void SetincInstance(devinf *p);
void ResetincInstance();
devinf *incInstance();
