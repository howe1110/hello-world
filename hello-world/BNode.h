#pragma once
#include "BConnect.h"
#include "BBase.h"
#include <vector>
#include <queue>
#include <set>
#include <map>

typedef std::string IDtype;

enum NodeState
{
    eOffLine,
    eJoining,
    eOnLine
};


const IDtype INVALID_NODEID = "0";

class BNode : public BBase
{
    typedef void (BNode::*NodeFunc)(BConnection*, const std::string&);

private:
    /* data */
public:
    BNode(/* args */);
    ~BNode();

public:
    static BNode *instance();

public:
    void Start();

public:
    bool StartListen();
    void Proc();
    void Close();
    void parse(BConnection* conn, const std::string &line);
    SOCKET Connect(const std::string &server, const std::string &port);

public:
    void Show();

public:
    void handleSuccessorReq(BConnection* conn, const std::string &s);
    void handleSuccessorRsp(BConnection* conn, const std::string &s);
    void hanldeAskPredecessorReq(BConnection* conn, const std::string &s);
    void hanldeAskPredecessorRsp(void *msg);
    void handleJoinReq(BConnection* conn, const std::string &s);
    void handleJoinPsp(BConnection* conn, const std::string &s);
    void handleStabilizeReq(BConnection* conn, const std::string &s);
    void handleStabilizeRsp(BConnection* conn, const std::string &s);

public:
    SOCKET Connect(IDtype id);
    void StartJoin(IDtype id);
    void join(IDtype id);
    void exit();
    void stabilization();
    void sendData(IDtype id, const std::string &msg);

private:
    IDtype _predecessor;
    IDtype _successor;

private:
    FD_SET getFdSet();
    FD_SET getWriteSet();
    void handleReadSockets(FD_SET fds);
    void handleWriteSocket(FD_SET fds);
    void handleErrorSocket(FD_SET fds);
    IDtype getIdentify(const std::string s);
    void check();

private:
    SOCKET ListenSocket;
    std::set<BConnection*> connSoc;
    struct sockaddr_in *_listen_addr;
    std::string _listen_port;

public:
    IDtype _id;
    std::string _name;

private:
    bool _start;
    NodeState _state;
    std::map<std::string, NodeFunc> _nodefuncmap;
};
