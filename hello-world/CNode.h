#pragma once
#include "BConnect.h"
#include "BBase.h"
#include "tx_queue.h"
#include "tx_ref.h"

#include <winsock2.h>
#include <set>
#include <map>

class CNode;
typedef void (CNode::*NodeFunc)(const ptxmsg);

    
typedef txRefPtr<BConnection> BConnectionPtr;

class CNode : public BBase
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
    virtual void ReceiveMessage(ptxmsg pMsg);
    virtual void SendBMessage(size_t linkid, const msgtype mt, void *pData, size_t datalen);
    void ProcMessage();
private:
    virtual void handleMessage(ptxmsg pmsg);

private:
    virtual void handleShakehand(const ptxmsg msg);

private:
    tx_queue<ptxmsg> _msgqueue;//接收队列
private:
    FD_SET getFdSet();
    FD_SET getWriteSet();
    void handleReadSockets(FD_SET fds);
    void handleWriteSocket(FD_SET fds);
    void handleErrorSocket(FD_SET fds);
private:
    bool addConntion(SOCKET st);
private:
    std::map<size_t, BConnectionPtr> connSoc;
    struct sockaddr_in *_listen_addr;
    std::string _listen_port;

private:
    bool _start;

protected:
    std::map<unsigned int, NodeFunc> _nodefuncmap;
};


