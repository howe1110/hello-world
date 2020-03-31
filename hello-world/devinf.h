#pragma once

class devinf
{
public:
    virtual ~devinf(){};

public:
    virtual INT getaddrinfoI(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult) = 0;
    virtual VOID freeaddrinfoI(PADDRINFOA pAddrInfo) = 0;
    virtual SOCKET socketI(int af, int type, int protocol) = 0;
    virtual int ioctlsocketI(SOCKET s, long cmd, u_long *argp) = 0;
    virtual int bindI(SOCKET s, struct sockaddr FAR *name, int namelen) = 0;
    virtual int listenI(SOCKET s, int backlog) = 0;
    virtual int selectI(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) = 0;
    virtual SOCKET acceptI(SOCKET s, struct sockaddr *addr, int *addrlen) = 0;
    virtual int connectI(SOCKET s, const struct sockaddr *name, int namelen) = 0;
    virtual int recvI(SOCKET s, char *buf, int len, int flags) = 0;
    virtual int sendI(SOCKET s, char *buf, int len, int flags) = 0;
    virtual int shutdownI (SOCKET s, int how) = 0;
    virtual int closesocketI(SOCKET s) = 0;
};

void SetincInstance(devinf* p);
devinf* incInstance();
