#include "CommonDef.h"
#include <string>
#include <map>
#include "tx_msg.h"
#include "tx_base.h"
#include "comworker.h"
#include "listen_worker.h"

const IDtype INVALID_NODEID = 0;
const unsigned int RING_BIT_SIZE = 32;

class StarNode : public comworker
{
private:
    /* data */
    IDtype _predecessor;
    IDtype _successor;
    IDtype _id;
    std::string _name;
    std::map<unsigned int, IDtype> _fingertable;

private:
    listen_worker _listener;
public:
    static StarNode &instance();
private:
    void handlemessage(tlinkptr plink, ptxmsg pMsg);
public:
    void Start();

public:
    void Show();

public:
    StarNode(/* args */);
    ~StarNode();

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
    void check();
    IDtype findSuccessor(IDtype);
    void fixFingerTable();
    IDtype getClosestNodeInFingerTable(IDtype id);
};
