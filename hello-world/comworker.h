#pragma once

#include "tx_worker.h"
#include "tx_link.h"
#include "tx_msg.h"
#include <ws2tcpip.h>
#include <map>

class comworker : public tx_worker_base
{
private:
    /* data */
    std::map<size_t, tlinkptr> _socketmap;
    tx_queue<SOCKET> _socketqueue;

public:
    comworker(/* args */);
    ~comworker();

public:
    void PostSocket(SOCKET st);

private:
    void proclinkqueue();
    FD_SET getFdSet();
    FD_SET getWriteSet();
    void handleReadSockets(FD_SET fds);
    void handleWriteSocket(FD_SET fds);
    void handleErrorSocket(FD_SET fds);

private:
    void close();

public:
    virtual void handlemessage(tlinkptr plink, ptxmsg pMsg) = 0;

public:
    void proc();
    void stop();
};

