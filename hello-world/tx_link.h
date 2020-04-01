#pragma once
#include "BBase.h"
#include <winsock2.h>
#include <string>
#include <vector>
#include "tx_ref.h"
#include "tx_msg.h"
#include "tx_base.h"

typedef unsigned int IDtype;
typedef unsigned int linkidtype;

enum ConnState
{
    eDisconnect,
    eConnecting,
    eConnected
};

const std::size_t send_buf_max = 4096;
const char msgidentfy[4] = {0x1e, 0x1e, 0x1e, 0x1e};

const int maxidletimes = 10;

enum linkerrorcode
{
    TXLNKCONNERR = 0,
    TXLNKRECVTO,
    TXLNKRECVERR,
    TXLNKSNDERR
};

class tlink : tx_base
{
private:
    size_t _id;
    static size_t _linkidhead;

private:
    SOCKET _socket;
    bool _blockmode;

private:
    std::string _server;
    std::string _port;
    ConnState _state;
    int idletimes;

private:
    size_t _recvmsgcount; //接收到的消息数量

public:
    tlink(SOCKET s, bool blk);
    virtual ~tlink();

public:
    int SendData(const void *buf, const msgtype mt, const size_t datalen);

public:
    int Recv();
    int RecvMessage(ptxmsg *ppMsg, size_t &len);
    int Send();
    void HandleError();
    //
    int GetBufPos();
    char *GetDataPos();
    bool Parse(ptxmsg *ppMsg, size_t &len);

private:
    void *allocBNodeMsg(size_t len);
    bool Parse(char **ppPuf, size_t datalen, ptxmsg *ppMsg, size_t &len);

public:
    void toString();
    size_t getId() { return _id; };

public:
    SOCKET GetSocket() const;
    ConnState GetState() const;
    int SendBufSize() const;
    bool CanWrite() const;
    bool IsTimeout();
    void Idle();

private:
    std::vector<char> _sendbuf;
    int _epos;
    int _spos;
    const static int sendbufthreshold = 1024;
    //
    bool _initized;
    const static int recvbuflen = 4096;
    const static int recvbufthreshold = 1024;
    std::vector<char> _recvbuf;
    char *_revdata;
    int _recvbufpos;
};

typedef txRefPtr<tlink> tlinkptr;