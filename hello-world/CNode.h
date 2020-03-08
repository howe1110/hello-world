#pragma once
#include "BConnect.h"
#include "BBase.h"
#include "tx_queue.h"

#include <winsock2.h>
#include <set>
class CNode;
typedef void (CNode::*NodeFunc)(const BNODE_MSG *);
class CNode: public BBase
{
public:
    CNode(/* args */);
    ~CNode();
public:
    bool Initialize();
    void Release();
    void Start();
private:
    virtual bool isRunning()
    {
        return _start;
    };
public:
    void StartListen();
    void check();
    void Proc();
    void Close();
    void Connect(const std::string &server, const std::string &port);
public:
    virtual void ReceiveMessage(PBNODE_MSG pMsg);
private:
    void ProcMessage();
    virtual void handleMessage(BNODE_MSG *pmsg);
private:
    tx_queue<PBNODE_MSG> _msgqueue;
private:
    FD_SET getFdSet();
    FD_SET getWriteSet();
    void handleReadSockets(FD_SET fds);
    void handleWriteSocket(FD_SET fds);
    void handleErrorSocket(FD_SET fds);
private:
    std::set<BConnection *> connSoc;
    struct sockaddr_in *_listen_addr;
    std::string _listen_port;
private:
    bool _start;
protected:
    std::map<unsigned int, NodeFunc> _nodefuncmap;
};
