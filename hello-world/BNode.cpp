#include "BNode.h"
#include "Hash.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <process.h>
#include <string>
#include <semaphore.h>
#include <iostream>
#include <sstream>
#include <time.h>

extern sem_t app_sem;

#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall StartRecvT(void *para)
{
    BNode *p = (BNode *)(para);
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
    BNode *pserverins = (BNode *)p;
    if (pserverins == nullptr)
    {
        return -1;
    }
    pserverins->StartListen();
    return 0;
}

void BNode::Start()
{
    int iResult;

    WSADATA wsaData;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return;
    }

    unsigned ret;
    sem_wait(&app_sem);
    _beginthreadex(0, 0, StartRecvT, (void *)this, 0, &ret);
    sem_wait(&app_sem);
    _beginthreadex(0, 0, StartListenT, (void *)this, 0, &ret);
    sem_wait(&app_sem);
    printf("Node started.\n");
}

BNode::BNode(/* args */)
{
    srand(time(NULL));
    std::stringstream ss;
    ss << rand() % 40000 + 10000;
    _listen_port = ss.str();

    _id = _listen_port;
    _successor = _id;
    _predecessor = _id;

    _nodefuncmap["successorReq"] = &BNode::handleSuccessorReq;
    _nodefuncmap["successorRsq"] = &BNode::handleSuccessorRsp;
    _nodefuncmap["joinReq"] = &BNode::handleJoinReq;
    _nodefuncmap["joinRsp"] = &BNode::handleJoinPsp;
}

BNode::~BNode()
{
    Close();
}

BNode *BNode::instance()
{
    static BNode ins;
    return &ins;
}

IDtype BNode::getIdentify(const std::string s)
{
    unsigned int id = RSHash(s);
    std::stringstream ss;
    ss << id;
    return ss.str();
}

void BNode::handleSuccessorReq(BConnection *conn, const std::string &s)
{
    Trace("Handle successor request.");
    std::string id = s;
    std::string rsp;
    if (_id == _successor && _successor == _predecessor)
    {
        rsp = "successorRsq " + _id;
        conn->SendData(rsp);
    }
    else if (_id < id && id < _successor)
    {
        rsp = "successorRsq " + _successor;
        conn->SendData(rsp);
    }
    else
    {
        sendData(_successor, s);
    }
}

void BNode::handleSuccessorRsp(BConnection *conn, const std::string &s)
{
    Trace("Handle successor response, successor {%s}", s.c_str());
    join(s);
}

void BNode::handleJoinReq(BConnection *conn, const std::string &s)
{
    Trace("Handle join request.");
    _predecessor = s;
    std::string sRsp = "joinRsp " + _id;
    conn->SendData(sRsp);
}

void BNode::handleJoinPsp(BConnection *conn, const std::string &s)
{
    Trace("Handle join rsponse.");
    _successor = s;
    Trace("Join sunncessful.");
}

void BNode::StartJoin(IDtype id)
{
    std::string msg = "successorReq " + _id;
    sendData(id, msg);
}

void BNode::join(IDtype id)
{
    std::string msg = "joinReq " + _id;
    sendData(id, msg);
}

void BNode::exit()
{
}

void BNode::stabilization()
{
}

void BNode::sendData(IDtype id, const std::string &msg)
{
    std::map<IDtype, BConnection *>::iterator it = connSoc.find(id);
    if (it == connSoc.end())
    {
        SOCKET st = Connect(id);
        if (st == INVALID_SOCKET)
        {
            return;
        }
        connSoc[id] = new BConnection(st);
    }
    connSoc[id]->SendData(msg);
}

void BNode::Show()
{
    printf("LocalNode:{%s}\n", _listen_port.c_str());
    sockaddr_in peeraddr;
    int size = sizeof(peeraddr);
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        it->second->toString();
    }
    Trace("Predecessor: {%s}, Successor: {%s}", _predecessor.c_str(), _successor.c_str());
}

bool BNode::StartListen()
{
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, _listen_port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return false;
    }

    _listen_addr = (struct sockaddr_in *)result->ai_addr;
    printf("start to listen on address %s:%u\n", inet_ntoa(_listen_addr->sin_addr), ntohs(_listen_addr->sin_port));

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    sem_post(&app_sem);

    socklen_t len;
    struct sockaddr_storage addr;
    u_short port;
    char ipstr[INET6_ADDRSTRLEN];

    len = sizeof addr;
    while (true)
    {
        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr *)&addr, &len);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        // set to noblock mode
        ULONG NonBlock = 1;
        if (ioctlsocket(ClientSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
        {
            printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            return false;
        }

        char saddr[INET6_ADDRSTRLEN] = {0};

        struct sockaddr_in *s = (struct sockaddr_in *)&addr;

        Trace("Client from %s:%d connected.", inet_ntoa(s->sin_addr), ntohs(s->sin_port));

        std::stringstream ss;
        ss << port;
        connSoc[ss.str()] = new BConnection(ClientSocket);
    }
}

SOCKET BNode::Connect(const std::string &server, const std::string &port)
{
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(server.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return INVALID_SOCKET;
    }

    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)result->ai_addr;

    SOCKET st = INVALID_SOCKET;
    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        st = socket(ptr->ai_family, ptr->ai_socktype,
                    ptr->ai_protocol);
        if (st == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            return false;
        }

        // Connect to server.
        iResult = connect(st, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(st);
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (st == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        closesocket(st);
        return false;
    }

    // set to noblock mode
    ULONG NonBlock = 1;
    if (ioctlsocket(st, FIONBIO, &NonBlock) == SOCKET_ERROR)
    {
        printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
        closesocket(st);
        return INVALID_SOCKET;
    }

    Trace("Connect to %s:%s ok.\n", server.c_str(), port.c_str());
    return st;
}

FD_SET BNode::getFdSet()
{
    FD_SET fds_;
    FD_ZERO(&fds_);
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        FD_SET(it->second->GetSocket(), &fds_);
    }
    return fds_;
}

FD_SET BNode::getWriteSet()
{
    FD_SET fds_;
    FD_ZERO(&fds_);
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        if (it->second->SendBufSize() > 0 && it->second->GetState() == eConnected)
        {
            FD_SET(it->second->GetSocket(), &fds_);
        }
    }
    return fds_;
}

void BNode::handleReadSockets(FD_SET fds)
{
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end();)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            std::string s = it->second->Recv();
            Trace(s.c_str());
            if (s.length() > 0)
            {
                parse(it->second, s);
            }
        }
        if (it->second->GetState() == eDisconnect)
        {
            connSoc.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void BNode::handleWriteSocket(FD_SET fds)
{
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            it->second->HandleWrite();
        }
    }
}

void BNode::handleErrorSocket(FD_SET fds)
{
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            it->second->HandleError();
        }
    }
}

void BNode::Close()
{
    for (std::map<IDtype, BConnection *>::iterator it = connSoc.begin(); it != connSoc.end(); ++it)
    {
        int iResult = shutdown(it->second->GetSocket(), SD_BOTH);
        if (iResult == SOCKET_ERROR)
        {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(it->second->GetSocket());
            return;
        }
        delete it->second;
    }
    connSoc.clear();
}

void BNode::parse(BConnection *conn, const std::string &line)
{
    std::vector<std::string> paras;
    std::string funcname;
    strsplit(line, funcname, paras);
    if (funcname.empty())
    {
        return;
    }
    std::map<std::string, NodeFunc>::iterator pos = _nodefuncmap.find(funcname);
    if (pos == _nodefuncmap.end())
    {
        Trace("Invalid command.");
        return;
    }

    NodeFunc func = pos->second;

    (this->*func)(conn, paras[0]);
}

void BNode::Proc()
{
    TIMEVAL tval;
    tval.tv_sec = 1;
    tval.tv_usec = 0;
    int ret = 0;
    _start = true;

    sem_post(&app_sem);
    while (_start)
    {
        FD_SET fds_r = getFdSet();
        FD_SET fds_w = getWriteSet();
        FD_SET fds_e = getFdSet();
        ret = select(0, &fds_r, (fd_set *)&fds_w, (fd_set *)&fds_e, &tval);
        if (ret == SOCKET_ERROR)
        {
            Sleep(10000);
            continue;
        }
        if (ret = 0)
        {
            Sleep(1000);
            continue;
        }
        handleReadSockets(fds_r);
        handleWriteSocket(fds_w);
        handleErrorSocket(fds_e);
    }
    Close();
}

//////////////////

SOCKET BNode::Connect(IDtype id)
{
    return Connect("192.168.1.81", id);
}
