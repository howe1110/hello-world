#include "listen_worker.h"
#include "devinf.h"
#include "comworker.h"
listen_worker::listen_worker(/* args */) : _listen_port("55555"), _comworker(nullptr), _lsocket(INVALID_SOCKET), tx_worker_base("listen_worker")
{
}

listen_worker::~listen_worker()
{
}

void listen_worker::setcommwoker(comworker *cwk)
{
    _comworker = cwk;
}

void listen_worker::handleconnect(SOCKET st)
{
    if (_comworker != nullptr)
    {
        _comworker->PostSocket(st);
    }
}

void listen_worker::stop()
{
    if (_lsocket != INVALID_SOCKET)
    {
        Trace("Close listen socket.");
        shutdown(_lsocket, SD_BOTH);
        closesocket(_lsocket);
    }
    tx_worker_base::stop();
}

void listen_worker::proc()
{
    struct addrinfo *result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    if(_lsocket != INVALID_SOCKET)
    {
        incInstance()->closesocketI(_lsocket);
        _lsocket = INVALID_SOCKET;
    }
    // Resolve the local address and port to be used by the server
    int iResult = incInstance()->getaddrinfoI(NULL, _listen_port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        return;
    }
    if (result == nullptr)
    {
        printf("result is null.");
        return;
    }

    _listen_addr = (struct sockaddr_in *)result->ai_addr;

    Trace("Start to listen on address %s:%u\n", inet_ntoa(_listen_addr->sin_addr), ntohs(_listen_addr->sin_port));

    SOCKET ListenSocket = incInstance()->socketI(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        incInstance()->freeaddrinfoI(result);
        return;
    }

    // Setup the TCP listening socket
    iResult = incInstance()->bindI(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        incInstance()->freeaddrinfoI(result);
        incInstance()->closesocketI(ListenSocket);
        return;
    }

    incInstance()->freeaddrinfoI(result);

    if (incInstance()->listenI(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        incInstance()->closesocketI(ListenSocket);
        return;
    }

    Trace("Listening on %s:%u success.", inet_ntoa(_listen_addr->sin_addr), ntohs(_listen_addr->sin_port));

    socklen_t len;
    struct sockaddr_storage addr;
    u_short port;
    char ipstr[INET6_ADDRSTRLEN];

    len = sizeof addr;
    
    _lsocket = ListenSocket;
    while (startswitch())
    {
        // Accept a client socket
        SOCKET ClientSocket = incInstance()->acceptI(_lsocket, (struct sockaddr *)&addr, &len);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            continue;
        }
        // set to noblock mode
        ULONG NonBlock = 1;
        if (incInstance()->ioctlsocketI(ClientSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
        {
            printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
            incInstance()->closesocketI(ClientSocket);
            continue;
        }

        handleconnect(ClientSocket);
        
        char saddr[INET6_ADDRSTRLEN] = {0};
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        Trace("Client from %s:%d connected.", inet_ntoa(s->sin_addr), ntohs(s->sin_port));
    }
}
