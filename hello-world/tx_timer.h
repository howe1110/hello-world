#pragma once

#include "tx_worker.h"

class tx_timer : public tx_worker_base
{
private:
    /* data */
    std::mutex _mtx;

public:
    tx_timer(/* args */);
    ~tx_timer();

public:
    void proc();

private:
    txmsgptr createtimermsg();
    void handletimer();
};
