#include "BConnect.h"
#include "Hash.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

#define DEFAULT_BUFLEN 512
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

BConnection::BConnection(SOCKET s) : _socket(s), _state(eConnected), _pos(0), _sendbuf_size(0), _initized(false)
{
    _sendbuf.resize(send_buf_max);
}

BConnection::BConnection(const BConnection &r)
{
    _socket = r.GetSocket();
    _state = r.GetState();
}

BConnection::~BConnection()
{
}

std::string BConnection::GetID() const
{
    return _id;
}

SOCKET BConnection::GetSocket() const
{
    return _socket;
}

ConnState BConnection::GetState() const
{
    return _state;
}

int BConnection::SendBufSize() const
{
    return _sendbuf_size;
}

bool BConnection::CanWrite() const
{
    if (_state == eConnecting)
    {
        return true;
    }
    if (_state == eConnected && _sendbuf_size > 0)
    {
        return true;
    }
    return false;
}

int BConnection::SendData(const std::string &s)
{
    if (_state != eConnected)
    {
        return -1;
    }
    if (_pos + _sendbuf_size + s.size() > send_buf_max)
    {
        return -1;
    }

    Trace("Send data %s", s.c_str());
    
    memcpy_s((void *)((char *)&_sendbuf[0] + _sendbuf_size), send_buf_max - _sendbuf_size, s.c_str(), s.size());
    _sendbuf_size += s.size();
    _sendbuf[_sendbuf_size] = 0;
    _sendbuf_size += 1;

    return s.size();
}

std::string BConnection::Recv()
{
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult = 0;
    std::string ret;
    // Receive until the peer closes the connection
    do
    {
        iResult = recv(_socket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            std::string t;
            t.assign(recvbuf, iResult);
            ret += t;
        }
        else if (iResult == 0)
        {
            Trace("Connection closed\n");
            _state = eDisconnect;
        }
    } while (iResult > 0);
    
    return ret;
}

int BConnection::HandleWrite()
{
    int iResult = 0;
    do
    {
        iResult = send(_socket, (const char *)&_sendbuf[_pos], _sendbuf_size, 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            break;
        }
        _pos += iResult;
        _sendbuf_size -= iResult;
        if (_sendbuf_size <= 0)
        {
            break;
        }
    } while (iResult > 0 & _sendbuf_size > 0);

    memmove_s((void *)&_sendbuf[0], send_buf_max, (char *)&_sendbuf[0] + _pos, _sendbuf_size);
    _pos = 0;

    return iResult;
}

void BConnection::HandleError()
{
}

void BConnection::Disconnect()
{
    // shutdown the connection since no more data will be sent
    int iResult = shutdown(_socket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(_socket);
        return;
    }
}

void BConnection::toString()
{
    printf("ID:{%s}, server:{%s}, port:{%s}, pos:{%d}, sendbuf_size{%d}, state:{%d}, socket:{%d}\n", _id.c_str(), _server.c_str(), _port.c_str(), _pos, _sendbuf_size, _state, _socket);
}
