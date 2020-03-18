#include "tcpcomm.h"

tcpcomm::tcpcomm(/* args */)
{
}

tcpcomm::~tcpcomm()
{
}

INT tcpcomm::getaddrinfoI(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult)
{
    return getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
}

VOID tcpcomm::freeaddrinfoI(PADDRINFOA pAddrInfo)
{
    freeaddrinfo(pAddrInfo);
}

SOCKET tcpcomm::socketI(int af, int type, int protocol)
{
    return socket(af, type, protocol);
}

int tcpcomm::ioctlsocketI(SOCKET s, long cmd, u_long *argp)
{
    return ioctlsocket(s, cmd, argp);
}

int tcpcomm::bindI(SOCKET s, struct sockaddr FAR *name, int namelen)
{
    return bind(s, name, namelen);
}

int tcpcomm::listenI(SOCKET s, int backlog)
{
    return listen(s, backlog);
}

int tcpcomm::selectI(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    return select(nfds, readfds, writefds, exceptfds, timeout);
}

SOCKET tcpcomm::acceptI(SOCKET s, struct sockaddr *addr, int *addrlen)
{
    return accept(s, addr, addrlen);
}

int tcpcomm::connectI(SOCKET s, const struct sockaddr *name, int namelen)
{
    return connect(s, name, namelen);
}

int tcpcomm::recvI(SOCKET s, char *buf, int len, int flags)
{
    return recv(s, buf, len, flags);
}

int tcpcomm::sendI(SOCKET s, char *buf, int len, int flags)
{
    return send(s, buf, len, flags);
}

int tcpcomm::shutdownI(SOCKET s, int how)
{
    return shutdown(s, how);
}

int tcpcomm::closesocketI(SOCKET s)
{
    return closesocket(s);
}
