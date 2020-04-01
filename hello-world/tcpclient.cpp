#include <ws2tcpip.h>
#include "tcpclient.h"
#include "devinf.h"
#include "tx_msg.h"

txcomclient::txcomclient(/* args */) : _plink(nullptr)
{
}

txcomclient::~txcomclient()
{
    if(_plink != nullptr)
    {
        delete _plink;
        _plink = nullptr;
    }
}

bool txcomclient::Connect(const std::string &server, const std::string &port)
{
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if(_plink != nullptr)
    {
        delete _plink;
        _plink = nullptr;
    }


    // Resolve the server address and port
    int iResult = incInstance()->getaddrinfoI(server.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return false;
    }

    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)result->ai_addr;

    SOCKET st = INVALID_SOCKET;
    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        st = incInstance()->socketI(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (st == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            continue;
        }
        // Connect to server.
        iResult = incInstance()->connectI(st, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            incInstance()->closesocketI(st);
            st = INVALID_SOCKET;
            continue;
        }
        break;
    }

    incInstance()->freeaddrinfoI(result);
    if (st == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        return false;
    }
    
    ULONG NonBlock = 1;
    if (incInstance()->ioctlsocketI(st, FIONBIO, &NonBlock) == SOCKET_ERROR)
    {
        printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
        incInstance()->closesocketI(st);
        return false;
    }

    _plink = new tlink(st, true);
    if (_plink == nullptr)
    {
        incInstance()->closesocketI(st);
        Trace("Connect to %s:%s error, new tlink failed.\n", server.c_str(), port.c_str());
        return false;
    }

    Trace("Connect to %s:%s ok.\n", server.c_str(), port.c_str());
    return true;
}

txcomresponse txcomclient::Request(const void *buf, const msgtype mt, const size_t datalen)
{
    size_t len = 0;

    if (_plink->SendData(buf, mt, datalen) < 0)
    {
        return txcomresponse(TXCOMRESPERR, nullptr);
    }
    ptxmsg pmsg;
    bool bRet = _plink->RecvMessage(&pmsg, len);
    if (bRet)
    {
        return txcomresponse(TXCOMRESPOK, pmsg);
    }
    
    return txcomresponse(TXCOMRESPERR, pmsg);;
}
