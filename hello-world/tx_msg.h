#pragma once

#include "CommonDef.h"
#include "tx_ref.h"

#include "stdlib.h"

#define __STDC_WANT_LIB_EXT1__ 1

#include <string.h>

typedef unsigned int txIdType;
//区间系统保留区间1-2048

const txIdType msgid_timerevent = 128;
const txIdType msgid_test_event = 256;

#pragma pack(1)

typedef struct txmsg
{
    txIdType msgid;
    size_t msglen;
    char data[0];

public:
    static txmsg *Create(void *data, txIdType id, size_t datalen)
    {
        size_t len = datalen + sizeof(txmsg);
        txmsg *pMsg = (txmsg *)malloc(len);
        memcpy(pMsg->data, data, datalen);
        pMsg->msgid = id;
        pMsg->msglen = datalen;
        return pMsg;
    }
    
    static txmsg *Clone(txmsg *p)
    {
        if (p == nullptr)
        {
            return p;
        }
        int len = sizeof(txmsg) + p->msglen;
        txmsg *pMsg = (txmsg *)malloc(len);
        memcpy(pMsg, p, len);
        return pMsg;
    };
} * ptxmsg;

#pragma pack()

const size_t NODE_COMMON_MSG_LEN = sizeof(txmsg);

class txmsgptr : public txRefPtr<txmsg>
{
private:
    /* data */
    size_t _linkid; //应用内部使用
public:
    txmsgptr() : txRefPtr<txmsg>()
    {
    }
    txmsgptr(ptxmsg pmsg) : txRefPtr<txmsg>(pmsg)
    {
    }
    ~txmsgptr()
    {
    }

public:
    size_t getlinkid()
    {
        return _linkid;
    }
    void setlinkid(size_t id)
    {
        _linkid = id;
    }
};
