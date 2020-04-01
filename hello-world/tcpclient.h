#pragma once
#include <string>
#include <vector>
#include "tx_base.h"
#include "tx_link.h"

enum txcomrespcode
{
    TXCOMRESPOK = 0,
    TXCOMRESPTM = 200,
    TXCOMRESPERR = 500
};

class txcomresponse : public txRefPtr<txmsg>
{
private:
    int _respcode;//相应码
public:
    txcomresponse(){};
    txcomresponse(int code, ptxmsg pmsg):txRefPtr<txmsg>(pmsg), _respcode(code)
    {

    }
    virtual ~txcomresponse(){};
public:
    int Respcode()
    {
        return _respcode;
    }
};

class txcomclient : public tx_base
{
private:
    tlink *_plink;
public:
    txcomclient(/* args */);
    ~txcomclient();

public:
    bool Connect(const std::string &server, const std::string &port);
    txcomresponse Request(const void *buf, const msgtype mt, const size_t datalen);
};
