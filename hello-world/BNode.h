#pragma once

#include "BConnect.h"
#include "CNode.h"
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <mutex>

//protoc --proto_path=./ --cpp_out=./ ./proto/Node.proto
typedef unsigned int IDtype;

enum NodeState
{
    eOffLine,
    eJoining,
    eOnLine
};

struct nodeinfo
{
    unsigned int nodeid;
    std::string address;
    std::string port;
};

struct successorReq
{
    nodeinfo node;
};

const IDtype INVALID_NODEID = 0;
const unsigned int RING_BIT_SIZE = 32;

class BNode : public CNode
{   
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
    void Show();

public:
    virtual void handleSuccessorReq(const txmsg *msg);
    virtual void handleSuccessorRsp(const txmsg *msg);
    virtual void handleJoinReq(const txmsg *msg);
    virtual void handleJoinPsp(const txmsg *msg);
    virtual void handleStabilizeReq(const txmsg *msg);
    virtual void handleStabilizeRsp(const txmsg *msg);

public:
    void StartJoin(const std::string &server, const std::string &port);
    void join(IDtype id);
    void exit();
    void stabilization();

private:
    IDtype _predecessor;
    IDtype _successor;

private:
    void check();
    IDtype findSuccessor(IDtype);
    void fixFingerTable();
    IDtype getClosestNodeInFingerTable(IDtype id);

public:
    IDtype _id;
    std::string _name;
    std::map<unsigned int, IDtype> _fingertable;

private:
    NodeState _state;
};
