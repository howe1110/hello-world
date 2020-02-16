#pragma once
#include "BBase.h"
#include <winsock2.h>
#include <string>
#include <vector>

enum ConnState
{
    eDisconnect,
    eConnecting,
    eConnected
};

const std::size_t send_buf_max = 65536;
const std::string shakehand = "hello server.";

const int maxidletimes = 10;

#define DEFAULT_BUFLEN 512

class BConnection : BBase
{
private:
    /* data */
public:
    BConnection(){};
    BConnection(SOCKET s);
    BConnection(const BConnection &r);
    virtual ~BConnection();

public:
    int SendData(const std::string &s);

public:
    std::string Recv();
    int HandleWrite();
    void HandleError();
    void Disconnect();

public:
    void toString();

public:
    SOCKET GetSocket() const;
    ConnState GetState() const;
    int SendBufSize() const;
    bool CanWrite() const;
    std::string GetID() const;
    bool IsTimeout();
    void Idle();

private:
    SOCKET _socket;

private:
    std::string _id;
    std::string _server;
    std::string _port;
    ConnState _state;
    int idletimes;

private:
    std::vector<char> _sendbuf;
    int _pos;
    int _sendbuf_size;
    bool _initized;
    const int recvbuflen = DEFAULT_BUFLEN;
};
