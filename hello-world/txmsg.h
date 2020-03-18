#pragma once
#include "CommonDef.h"
#include "tx_ref.h"

#include "stdlib.h"

#pragma pack(1)

typedef struct txmsg
{
    size_t linkid;//应用内部使用
    unsigned short msgid;
    IDtype sendid;
    IDtype recvid;
    size_t msglen;
    char data[0];
public:
    static txmsg *Clone(txmsg *p)
    {
        if(p == nullptr)
        {
            return p;
        }
        int len = sizeof(txmsg) + p->msglen;
        txmsg * pMsg = (txmsg *)malloc(len);
        memcpy_s(pMsg, len, p, len);
        return pMsg;
    };
} *ptxmsg;


const size_t NODE_COMMON_MSG_LEN = sizeof(txmsg);

typedef txRefPtr<txmsg> txMsgSharePtr;

