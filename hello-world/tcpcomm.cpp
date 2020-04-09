#include "tcpcomm.h"
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
tcpcomm::tcpcomm(/* args */)
{
}

tcpcomm::~tcpcomm()
{
}

int tcpcomm::getaddrinfoI(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    return getaddrinfo(node, service, hints, res);
}

void tcpcomm::freeaddrinfoI(struct addrinfo *res)
{
    freeaddrinfo(res);
}

int tcpcomm::socketI(int domain, int type, int protocol)
{
    return socket(domain, type, protocol);
}

int tcpcomm::setsocketblock(int s, bool block)
{
    int flags = fcntl(s, F_GETFL, 0);
    if (block)
    {
        flags = flags | ~O_NONBLOCK;
    }
    else
    {
        flags = flags | O_NONBLOCK;
    }
    return fcntl(s, F_SETFL, flags);
}

int tcpcomm::bindI(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return bind(sockfd, addr, addrlen);
}

int tcpcomm::listenI(int sockfd, int backlog)
{
    return listen(sockfd, backlog);
}

int tcpcomm::selectI(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    return select(nfds, readfds, writefds, exceptfds, timeout);
}

int tcpcomm::acceptI(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    return accept(sockfd, addr, addrlen);
}

int tcpcomm::connectI(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return connect(sockfd, addr, addrlen);
}

int tcpcomm::recvI(int sockfd, void *buf, size_t len, int flags)
{
    return recv(sockfd, buf, len, flags);
}

int tcpcomm::sendI(int sockfd, const void *buf, size_t len, int flags)
{
    return send(sockfd, buf, len, flags);
}

int tcpcomm::shutdownI(int sockfd, int how)
{
    return shutdown(sockfd, how);
}

int tcpcomm::closesocketI(int fd)
{
    if (fd == INVALID_SOCKET)
    {
        return 0;
    }
    return close(fd);
}

int tcpcomm::setsocketreuseaddr(int fd)
{
    int reuse = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}
