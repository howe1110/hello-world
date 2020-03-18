#pragma once
#include "BBase.h"
#include <winsock2.h>
#include <string>
#include <vector>
#include "tx_ref.h"
#include "txmsg.h"

typedef unsigned int IDtype;


enum ConnState
{
    eDisconnect,
    eConnecting,
    eConnected
};

const std::size_t send_buf_max = 4096;
const std::string shakehand = "hello server.";
const char msgidentfy[4] = {0x1e, 0x1e, 0x1e, 0x1e};

const int maxidletimes = 10;

class BConnection : BBase
{
private:
    /* data */
public:
    BConnection();
    BConnection(SOCKET s);
    BConnection(const BConnection &r);
    virtual ~BConnection();

public:
public:
    void *allocBNodeMsg(size_t len);
    void sendData(IDtype id, const void *buf, const msgtype mt, const size_t datalen);

public:
    void Recv();
    void HandleWrite();
    void HandleError();
    //
    int GetBufPos();
    char *GetDataPos();
    bool Parse(ptxmsg *ppMsg, size_t &len);

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
    size_t _id;
    static size_t _linkidhead;

private:
    SOCKET _socket;

private:
    std::string _server;
    std::string _port;
    ConnState _state;
    int idletimes;

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

private:
    size_t _recvmsgcount; //接收到的消息数量
};
