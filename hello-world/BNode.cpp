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
#include <math.h>
#include <algorithm>


extern sem_t app_sem;



BNode::BNode(/* args */) : _predecessor(INVALID_NODEID), _successor(INVALID_NODEID)
{
    srand(time(NULL));
    std::stringstream ss;

    _id = rand() % 40000 + 10000;

    ss << _id;

    _successor = _id;
    _predecessor = _id;

    _nodefuncmap[successor_req] = (NodeFunc)&BNode::handleSuccessorReq;
    _nodefuncmap[successor_rsp] = (NodeFunc)&BNode::handleSuccessorRsp;
    _nodefuncmap[join_req] = (NodeFunc)&BNode::handleJoinReq;
    _nodefuncmap[join_rsp] = (NodeFunc)&BNode::handleJoinPsp;
    _nodefuncmap[stabilize_req] = (NodeFunc)&BNode::handleStabilizeReq;
    _nodefuncmap[stabilize_rsp] = (NodeFunc)&BNode::handleStabilizeRsp;
}

BNode::~BNode()
{
}

BNode *BNode::instance()
{
    static BNode ins;
    return &ins;
}

void BNode::StartJoin(IDtype id)
{

}

void BNode::handleSuccessorReq(const BNODE_MSG *msg)
{
    Trace("Handle successor request.");
}

void BNode::handleSuccessorRsp(const BNODE_MSG *msg)
{

}

void BNode::handleJoinReq(const BNODE_MSG *msg)
{

}

void BNode::handleJoinPsp(const BNODE_MSG *msg)
{

}

void BNode::handleStabilizeReq(const BNODE_MSG *msg)
{

}

void BNode::handleStabilizeRsp(const BNODE_MSG *msg)
{

}

void BNode::join(IDtype id)
{
}

void BNode::exit()
{
}

void BNode::stabilization()
{
}

void BNode::Show()
{
    Trace("LocalNode:{%u}\n", _id);
    Trace("Predecessor: {%u}, Successor: {%s}", _predecessor, _successor);
}

IDtype BNode::findSuccessor(IDtype id)
{
    return INVALID_NODEID;
}

//_fingertable[i] = findSuccessor((_id + Power(2, i))% RING_BIT_SIZE);
void BNode::fixFingerTable()
{
    for (int i = 0; i < RING_BIT_SIZE; ++i)
    {
        IDtype rec = pow(2, i);
        IDtype next = INVALID_NODEID;
        if (std::numeric_limits<IDtype>::max() - rec > _id)
        {
            next = pow(2, i) - (std::numeric_limits<IDtype>::max() - _id);
        }
        else
        {
            next = _id + rec;
        }

        _fingertable[i] = findSuccessor(next);
        if (_fingertable[i] == _id) //第一个后继节点是自己的
        {
            break;
        }
    }
    for (int i = i + 1; i < RING_BIT_SIZE; i++) //后续每一个后继节点都是此节点
    {
        _fingertable[i] = _id;
    }
}

IDtype BNode::getClosestNodeInFingerTable(IDtype id)
{
    for (int i = RING_BIT_SIZE - 1; i >= 0; i--) //倒序查找，先大范围查找。
    {
        if (_fingertable[i] == INVALID_NODEID)
        {
            continue;
        }
        if (_fingertable[i] == _id)
        {
            /* code */
            continue;
        }
        if (id < _fingertable[i] && _fingertable[i] < _id)
        {
            return _fingertable[i];
        }
    }
    return _successor;
}
