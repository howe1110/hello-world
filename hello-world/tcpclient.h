#pragma once
#include <string>
#include <vector>
#include "tx_base.h"
#include "tx_link.h"

class txcomclient : public tx_base
{
private:
    tlink *_plink;
public:
    txcomclient(/* args */);
    ~txcomclient();

public:
    bool Connect(const std::string &server, const std::string &port);
    txRefPtr<txmsg> Request(const void *buf, const msgtype mt, const size_t datalen);
};
