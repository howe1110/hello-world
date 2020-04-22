#include "StarNode.h"
#include "tcpclient.h"
#include <algorithm>
#include <math.h>
#include "tx_env.h"

StarNode::StarNode(/* args */)
{
}

StarNode::~StarNode()
{
}

StarNode &StarNode::instance()
{
    static StarNode ins;
    return ins;
}

void StarNode::handlemessage(tlinkptr plink, ptxmsg pMsg)
{

}

void StarNode::Start()
{
    start();//启动消息处理线程
    _listener.setcommwoker(this);
    _listener.start();//启动侦听功能。
}

void StarNode::StartJoin(const std::string &server, const std::string &port)
{
    txcomclient client;
    bool bRet = client.Connect(server, port);
    if(!bRet)
    {
        return;
    }
}

void StarNode::handleSuccessorReq(const txmsg *msg)
{

}

void StarNode::handleSuccessorRsp(const txmsg *msg)
{

}

void StarNode::handleJoinReq(const txmsg *msg)
{

}

void StarNode::handleJoinPsp(const txmsg *msg)
{

}

void StarNode::handleStabilizeReq(const txmsg *msg)
{

}

void StarNode::handleStabilizeRsp(const txmsg *msg)
{

}

void StarNode::join(IDtype id)
{
}

void StarNode::exit()
{
}

void StarNode::stabilization()
{
}

void StarNode::Show()
{
    Trace("LocalNode:{%u}\n", _id);
    Trace("Predecessor: {%u}, Successor: {%s}", _predecessor, _successor);
}

IDtype StarNode::findSuccessor(IDtype id)
{
    return INVALID_NODEID;
}

//_fingertable[i] = findSuccessor((_id + Power(2, i))% RING_BIT_SIZE);
void StarNode::fixFingerTable()
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

IDtype StarNode::getClosestNodeInFingerTable(IDtype id)
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

