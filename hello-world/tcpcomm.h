#pragma once

#include "devinf.h"

class tcpcomm : public devinf
{
private:
    /* data */
public:
    tcpcomm(/* args */);
    ~tcpcomm();

public:
    virtual int getaddrinfoI(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
    virtual void freeaddrinfoI(struct addrinfo *res);
    virtual int socketI(int domain, int type, int protocol);
    virtual int setsocketblock(int s, bool block);
    virtual int bindI(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    virtual int listenI(int sockfd, int backlog);
    virtual int selectI(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    virtual int acceptI(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    virtual int connectI(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    virtual int recvI(int sockfd, void *buf, size_t len, int flags);
    virtual int sendI(int sockfd, const void *buf, size_t len, int flags);
    virtual int shutdownI(int sockfd, int how);
    virtual int closesocketI(int fd);
    virtual int setsocketreuseaddr(int sockfd);
};
