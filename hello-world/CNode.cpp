#include "CNode.h"
#include <ws2tcpip.h>

#include <semaphore.h>

extern sem_t app_sem;
#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall StartRecvT(void *para)
{
    CNode *p = (CNode *)(para);
    if (p == nullptr)
    {
        printf("fatal error 02.\n");
        return -1;
    }
    p->Proc();
    return 0;
}

unsigned __stdcall StartListenT(void *p)
{
    CNode *pserverins = (CNode *)p;
    if (pserverins == nullptr)
    {
        return -1;
    }
    pserverins->StartListen();
    return 0;
}

unsigned __stdcall StartProcMessageT(void *p)
{
    CNode *pserverins = (CNode *)p;
    if (pserverins == nullptr)
    {
        return -1;
    }
    pserverins->ProcMessage();
    return 0;
}

CNode::CNode()
{

}

CNode::~CNode()
{

}

bool CNode::Initialize()
{
    WSADATA wsaData;
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return false;
    }
    return true;
}
void CNode::Release()
{
    WSACleanup();
}

void CNode::Start()
{
    unsigned ret;
    sem_wait(&app_sem);
    _beginthreadex(0, 0, StartProcMessageT, (void *)this, 0, &ret);
    sem_wait(&app_sem);
    _beginthreadex(0, 0, StartRecvT, (void *)this, 0, &ret);
    sem_wait(&app_sem);
    _beginthreadex(0, 0, StartListenT, (void *)this, 0, &ret);
    sem_wait(&app_sem);
    printf("Node started.\n");
}

void CNode::ProcMessage()
{
    while (isRunning())
    {
        PBNODE_MSG msg;
        _msgqueue.read(msg, 1);
        handleMessage(msg);
    }
}

void CNode::handleMessage(BNODE_MSG *pMsg)
{
    if(pMsg == nullptr)
    {
        return;
    }
    std::map<unsigned int, NodeFunc>::iterator pos = _nodefuncmap.find(pMsg->msgid);
    if(pos == _nodefuncmap.end())
    {
        Trace("Invalid message id.");
        return;
    }
    NodeFunc func = pos->second;
    (this->*func)(pMsg);
}

void CNode::ReceiveMessage(PBNODE_MSG pMsg)
{
    _msgqueue.write(pMsg);
}

FD_SET CNode::getFdSet()
{
    FD_SET fds_;
    FD_ZERO(&fds_);
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        FD_SET((*it)->GetSocket(), &fds_);
    }
    return fds_;
}

FD_SET CNode::getWriteSet()
{
    FD_SET fds_;
    FD_ZERO(&fds_);
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        if ((*it)->SendBufSize() > 0 && (*it)->GetState() == eConnected)
        {
            FD_SET((*it)->GetSocket(), &fds_);
        }
    }
    return fds_;
}

void CNode::handleReadSockets(FD_SET fds)
{
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end();)
    {
        if (FD_ISSET((*it)->GetSocket(), &fds) > 0)
        {
            (*it)->Recv();
        }
        if ((*it)->GetState() == eDisconnect)
        {
            connSoc.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void CNode::handleWriteSocket(FD_SET fds)
{
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        if (FD_ISSET((*it)->GetSocket(), &fds) > 0)
        {
            (*it)->HandleWrite();
        }
    }
}

void CNode::handleErrorSocket(FD_SET fds)
{
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        if (FD_ISSET((*it)->GetSocket(), &fds) > 0)
        {
            (*it)->HandleError();
        }
    }
}

void CNode::Close()
{
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        int iResult = incInstance()->shutdownI((*it)->GetSocket(), SD_BOTH);
        if (iResult == SOCKET_ERROR)
        {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            incInstance()->closesocketI((*it)->GetSocket());
            return;
        }
        delete *it;
    }
    connSoc.clear();
}

void CNode::check()
{
    for (std::set<BConnection *>::iterator it = connSoc.begin(); it != connSoc.end();)
    {
        (*it)->Idle();
        if ((*it)->IsTimeout())
        {
            Trace("Time out.");
            // shutdown the connection since no more data will be sent
            int iResult = incInstance()->shutdownI((*it)->GetSocket(), SD_SEND);
            if (iResult == SOCKET_ERROR)
            {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
                incInstance()->closesocketI((*it)->GetSocket());
            }
            delete *it;
            connSoc.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void CNode::Proc()
{
    TIMEVAL tval;
    tval.tv_sec = 1;
    tval.tv_usec = 0;
    int ret = 0;
    _start = true;

    int count = 0;

    sem_post(&app_sem);
    while (isRunning())
    {
        FD_SET fds_r = getFdSet();
        FD_SET fds_w = getWriteSet();
        FD_SET fds_e = getFdSet();
        ret = incInstance()->selectI(0, &fds_r, (fd_set *)&fds_w, (fd_set *)&fds_e, &tval);
        if(ret > 0)
        {
            handleReadSockets(fds_r);
            handleWriteSocket(fds_w);
            handleErrorSocket(fds_e);
        }
        else
        {
            check();
        }
    }
    Close();
}

void CNode::Connect(const std::string &server, const std::string &port)
{
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = incInstance()->getaddrinfoI(server.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return;
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
        // set to noblock mode
        ULONG NonBlock = 1;
        if (incInstance()->ioctlsocketI(st, FIONBIO, &NonBlock) == SOCKET_ERROR)
        {
            printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
            incInstance()->closesocketI(st);
            st = INVALID_SOCKET;
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
        return;
    }
    BConnection *pconn = new BConnection(st, this);
    if (pconn == nullptr)
    {
        printf("New BConnection failed with error %d\n", WSAGetLastError());
        return;
    }
    connSoc.insert(pconn);
    Trace("Connect to %s:%s ok.\n", server.c_str(), port.c_str());
}

void CNode::StartListen()
{
    struct addrinfo *result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = incInstance()->getaddrinfoI(NULL, _listen_port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        return;
    }
    if(result == nullptr)
    {
        printf("result is null.");
        return;
    }

    _listen_addr = (struct sockaddr_in *)result->ai_addr;
    printf("start to listen on address %s:%u\n", inet_ntoa(_listen_addr->sin_addr), ntohs(_listen_addr->sin_port));

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

    sem_post(&app_sem);

    socklen_t len;
    struct sockaddr_storage addr;
    u_short port;
    char ipstr[INET6_ADDRSTRLEN];

    len = sizeof addr;
    while (isRunning())
    {
        // Accept a client socket
        SOCKET ClientSocket = incInstance()->acceptI(ListenSocket, (struct sockaddr *)&addr, &len);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            break;
        }
        // set to noblock mode
        ULONG NonBlock = 1;
        if (incInstance()->ioctlsocketI(ClientSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
        {
            printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
            incInstance()->closesocketI(ClientSocket);
            continue;
        }

        char saddr[INET6_ADDRSTRLEN] = {0};

        struct sockaddr_in *s = (struct sockaddr_in *)&addr;

        Trace("Client from %s:%d connected.", inet_ntoa(s->sin_addr), ntohs(s->sin_port));

        BConnection *pconn = new BConnection(ClientSocket, this);
        if (pconn == nullptr)
        {
            printf("New BConnection failed with error %d\n", WSAGetLastError());
            continue;
        }
        connSoc.insert(pconn);
    }
    incInstance()->closesocketI(ListenSocket);
}