#include "listen_worker.h"
#include "devinf.h"
#include "comworker.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "tx_env.h"

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

void listen_worker::handleconnect(int st)
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
        logerr("Close listen socket.");
        shutdown(_lsocket, SHUT_RDWR);
        close(_lsocket);
    }
    tx_worker_base::stop();
}

void listen_worker::proc()
{
    struct addrinfo *result = nullptr, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    logwarn("listen woker starting.");

    if(_lsocket != INVALID_SOCKET)
    {
        incInstance()->closesocketI(_lsocket);
        _lsocket = INVALID_SOCKET;
    }

    // Resolve the local address and port to be used by the server
    int iResult = incInstance()->getaddrinfoI(NULL, _listen_port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        logerr("getaddrinfo: %s\n", gai_strerror(iResult));
        return;
    }
    if (result == nullptr)
    {
        logerr("result is null.");
        return;
    }

    _listen_addr = (struct sockaddr_in *)result->ai_addr;

    logwarn("Start to listen on address %s:%u\n", inet_ntoa(_listen_addr->sin_addr), ntohs(_listen_addr->sin_port));

    int ListenSocket = incInstance()->socketI(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == -1)
    {
        logerr("Error at socket(): %d\n", gai_strerror(errno));
        incInstance()->freeaddrinfoI(result);
        return;
    }

    incInstance()->setsocketreuseaddr(ListenSocket);

    // Setup the TCP listening socket
    iResult = incInstance()->bindI(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        logerr("bind failed with error: %d\n", gai_strerror(errno));
        incInstance()->freeaddrinfoI(result);
        incInstance()->closesocketI(ListenSocket);
        return;
    }

    incInstance()->freeaddrinfoI(result);

    if (incInstance()->listenI(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        logerr("Listen failed with error: %ld\n", gai_strerror(errno));
        incInstance()->closesocketI(ListenSocket);
        return;
    }

    logwarn("Listening on %s:%u success.", inet_ntoa(_listen_addr->sin_addr), ntohs(_listen_addr->sin_port));

    socklen_t len;
    struct sockaddr_storage addr;
    u_short port;
    char ipstr[INET6_ADDRSTRLEN];

    len = sizeof addr;
    
    _lsocket = ListenSocket;
    while (startswitch())
    {
        // Accept a client socket
        int ClientSocket = incInstance()->acceptI(_lsocket, (struct sockaddr *)&addr, &len);
        if (ClientSocket == SOCKET_ERROR)
        {
            logerr("accept failed with error: %d\n", gai_strerror(errno));
            continue;
        }
        // set to noblock mode
        if (incInstance()->setsocketblock(ClientSocket, false) == SOCKET_ERROR)
        {
            logerr("setsocketblock() failed with error %d\n", gai_strerror(errno));
            incInstance()->closesocketI(ClientSocket);
            continue;
        }

        handleconnect(ClientSocket);
        
        char saddr[INET6_ADDRSTRLEN] = {0};
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        logtrace("Client from %s:%d connected.", inet_ntoa(s->sin_addr), ntohs(s->sin_port));
    }
}
