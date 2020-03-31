#pragma once
#include "CommonDef.h"
#include "tx_ref.h"

#include "stdlib.h"

typedef unsigned int txIdType;

#pragma pack(1)

typedef struct txmsg
{
    txIdType msgid;
    size_t msglen;
    char data[0];

public:
    static txmsg *Clone(txmsg *p)
    {
        if (p == nullptr)
        {
            return p;
        }
        int len = sizeof(txmsg) + p->msglen;
        txmsg *pMsg = (txmsg *)malloc(len);
        memcpy_s(pMsg, len, p, len);
        return pMsg;
    };
} * ptxmsg;

const size_t NODE_COMMON_MSG_LEN = sizeof(txmsg);

class txmsgptr : public txRefPtr<txmsg>
{
private:
    /* data */
    size_t _linkid; //应用内部使用
public:
    txmsgptr();
    txmsgptr(ptxmsg pmsg);
    ~txmsgptr();

public:
    size_t getlinkid();
    void setlinkid(size_t id);
};

txmsgptr::txmsgptr() : txRefPtr<txmsg>()
{
}

txmsgptr::txmsgptr(ptxmsg pmsg) : txRefPtr<txmsg>(pmsg)
{
}

txmsgptr::~txmsgptr()
{
}
size_t txmsgptr::getlinkid()
{
    return _linkid;
}

void txmsgptr::setlinkid(size_t id)
{
    _linkid = id;
}


