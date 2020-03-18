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
#include "network.h"

size_t BConnection::_linkidhead = 0;

BConnection::BConnection(SOCKET s) : _socket(s), _state(eConnected), idletimes(0), _spos(0), _epos(0), _initized(false)
{
    _sendbuf.resize(send_buf_max);
    _recvbuf.resize(recvbuflen);
    _recvbufpos = 0;
    _revdata = &_recvbuf[0];
    _recvmsgcount = 0;
    _id = _linkidhead++;
}

BConnection::BConnection(const BConnection &r)
{
    _socket = r.GetSocket();
    _state = r.GetState();
}

BConnection::BConnection()
{
}

BConnection::~BConnection()
{
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
    return (_epos - _spos);
}

int BConnection::GetBufPos()
{
    return _recvbufpos;
}

char *BConnection::GetDataPos()
{
    return _revdata;
}

bool BConnection::IsTimeout()
{
    return idletimes > maxidletimes;
}

void BConnection::Idle()
{
    idletimes++;
}

bool BConnection::CanWrite() const
{
    if (_state == eConnecting)
    {
        return true;
    }
    if (_state == eConnected && SendBufSize() > 0)
    {
        return true;
    }
    return false;
}

//考虑用线程消息池实现
void *BConnection::allocBNodeMsg(size_t len)
{
    if ((send_buf_max - _epos) <= sendbufthreshold) //剩余发送缓冲区空间不够
    {
        void* pSendData = &_sendbuf[0] +  + _spos;
        memmove_s(&_sendbuf[0], send_buf_max, pSendData, _spos);
        _epos -= _spos;
        _spos = 0;
    }

    if ((send_buf_max - _epos) < len) //剩余发送缓冲区空间不够
    {
        return nullptr;
    }

    void *buf = &_sendbuf[0] + _epos;
    _epos += len;
    return buf;
}

void BConnection::HandleWrite()
{
    if(_epos <= _spos)//没有要发送的数据
    {
        return;
    }

    idletimes = 0;
    int iResult = 0;
    char *pSendbuf = &_sendbuf[0] + _spos;
    iResult = incInstance()->sendI(_socket, pSendbuf, (_epos - _spos), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        return;
    }
    _spos += iResult;
}

void BConnection::sendData(IDtype id, const void *buf, const msgtype mt, const size_t datalen)
{
    if (buf == nullptr)
    {
        return;
    }

    size_t len = datalen + NODE_COMMON_MSG_LEN + sizeof(msgidentfy);

    txmsg *msg = (txmsg *)allocBNodeMsg(len);
    if (msg == nullptr)
    {
        return;
    }
    msg->msgid = mt;
    msg->msglen = datalen;

    memcpy_s(msg->data, len, buf, len);                                            //拷贝消息内容
    memcpy_s(msg->data + len, sizeof(msgidentfy), msgidentfy, sizeof(msgidentfy)); //加入消息尾标识
}

bool BConnection::Parse(ptxmsg *ppMsg, size_t &len)
{
    size_t pos = 0;
    char *bpos = (char *)_revdata;
    size_t datalen = &_recvbuf[0] + _recvbufpos - _revdata;
    Trace("datalen is %u", datalen);

    txmsg *pMsg = nullptr;

    while (pos + sizeof(msgidentfy) <= datalen)
    {
        int ret = memcmp(bpos + pos, msgidentfy, sizeof(msgidentfy));
        if (ret == 0) //找到消息结束标识符
        {
            _recvmsgcount++;
            len = bpos + pos - _revdata;
            pMsg = (txmsg *)_revdata;
            pos += sizeof(msgidentfy);
            _revdata += pos;
            break;//每次只收一个消息.
        }
        else
        {
            ++pos;
        }
    }
    
    if(pMsg != nullptr)
    {
        *ppMsg = pMsg;
        return true;
    }

    return false;
}


void BConnection::Recv()
{
    idletimes = 0;
    int iResult = 0;
    int recvcount = 0;

    char *buf = &_recvbuf[0] + _recvbufpos;

    int buflen = recvbuflen - _recvbufpos;

    if (buflen < recvbufthreshold) //小于阈值，开始整理缓冲区
    {
        size_t datalen = &_recvbuf[0] + _recvbufpos - _revdata;
        errno_t e = memmove_s(&_recvbuf[0], recvbuflen, _revdata, datalen);
        if (e != 0)
        {
            Trace("memmove_s error.");
            return;
        }
        _revdata = &_recvbuf[0];
        _recvbufpos = datalen;
    }

    // Receive until the peer closes the connection
    do
    {
        iResult = incInstance()->recvI(_socket, buf, buflen, 0);
        if (iResult > 0)
        {
            buf += iResult;
            buflen -= iResult;
            recvcount += iResult;
        }
        else if (iResult == 0)
        {
            Trace("Connection closed\n");
            _state = eDisconnect;
        }
    } while (iResult > 0);

    Trace("reiceive %d\n", recvcount);

    _recvbufpos += recvcount;

}

void BConnection::HandleError()
{
}

void BConnection::toString()
{
}
