#include "network.h"

class tcpcomm : public network
{
private:
    /* data */
public:
    tcpcomm(/* args */);
    ~tcpcomm();

public:
    virtual INT getaddrinfoI(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult);
    virtual VOID freeaddrinfoI(PADDRINFOA pAddrInfo);
    virtual SOCKET socketI(int af, int type, int protocol);
    virtual int ioctlsocketI(SOCKET s, long cmd, u_long *argp);
    virtual int bindI(SOCKET s, struct sockaddr FAR *name, int namelen);
    virtual int listenI(SOCKET s, int backlog);
    virtual int selectI(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout);
    virtual SOCKET acceptI(SOCKET s, struct sockaddr *addr, int *addrlen);
    virtual int connectI(SOCKET s, const struct sockaddr *name, int namelen);
    virtual int recvI(SOCKET s, char *buf, int len, int flags);
    virtual int sendI(SOCKET s, char *buf, int len, int flags);
    virtual int shutdownI (SOCKET s, int how);
    virtual int closesocketI(SOCKET s);
};
