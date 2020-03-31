#include "tx_worker.h"
#include "tx_msg.h"
#include "tx_queue.h"

class appwork : public tx_worker_base
{
private:
    /* data */
    tx_queue<txmsgptr> _msgqueue; //接收队列
public:
    appwork(/* args */);
    virtual ~appwork();

public:
    void proc();

public:
    virtual void handleMessage(txmsgptr pMsg);
    void postmessage(txmsgptr pMsg);
};
