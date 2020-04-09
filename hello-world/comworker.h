#pragma once

#include "tx_worker.h"
#include "tx_link.h"
#include "tx_msg.h"
#include <map>

class comworker : public tx_worker_base
{
private:
    /* data */
    std::map<size_t, tlinkptr> _socketmap;
    tx_queue<int> _socketqueue;

public:
    comworker(/* args */);
    ~comworker();

public:
    void PostSocket(int st);

private:
    void proclinkqueue();
    void getFdSet(fd_set& fds);
    void getWriteSet(fd_set& fds);
    void handleReadSockets(fd_set fds);
    void handleWriteSocket(fd_set fds);
    void handleErrorSocket(fd_set fds);

private:
    void close();

public:
    virtual void handlemessage(tlinkptr plink, ptxmsg pMsg) = 0;

public:
    void proc();
    void stop();
};

