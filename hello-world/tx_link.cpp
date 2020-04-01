#include "tx_link.h"
#include "Hash.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>

#include "devinf.h"

size_t tlink::_linkidhead = 0;

tlink::tlink(SOCKET s, bool blk) : _socket(s), _blockmode(blk), _state(eConnected), idletimes(0), _spos(0), _epos(0), _initized(false)
{
    _sendbuf.resize(send_buf_max);
    _recvbuf.resize(recvbuflen);
    _recvbufpos = 0;
    _revdata = &_recvbuf[0];
    _recvmsgcount = 0;
    _id = _linkidhead++;
}

tlink::~tlink()
{
    incInstance()->closesocketI(_socket);
}

SOCKET tlink::GetSocket() const
{
    return _socket;
}

ConnState tlink::GetState() const
{
    return _state;
}

int tlink::SendBufSize() const
{
    return (_epos - _spos);
}

int tlink::GetBufPos()
{
    return _recvbufpos;
}

char *tlink::GetDataPos()
{
    return _revdata;
}

bool tlink::IsTimeout()
{
    return idletimes > maxidletimes;
}

void tlink::Idle()
{
    idletimes++;
}

bool tlink::CanWrite() const
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
void *tlink::allocBNodeMsg(size_t len)
{
    if ((send_buf_max - _epos) <= sendbufthreshold) //剩余发送缓冲区空间不够
    {
        void *pSendData = &_sendbuf[0] + +_spos;
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

int tlink::Send()
{
    if (_epos <= _spos) //没有要发送的数据
    {
        return 0;
    }

    idletimes = 0;
    char *pSendbuf = &_sendbuf[0] + _spos;
    int iResult = incInstance()->sendI(_socket, pSendbuf, (_epos - _spos), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        return 0;
    }
    _spos += iResult;
    return iResult;
}

int tlink::SendData(const void *buf, const msgtype mt, const size_t datalen)
{
    if (buf == nullptr)
    {
        return -1;
    }

    size_t len = datalen + NODE_COMMON_MSG_LEN + sizeof(msgidentfy);

    txmsg *msg = (txmsg *)allocBNodeMsg(len);
    if (msg == nullptr)
    {
        return -1;
    }
    msg->msgid = mt;
    msg->msglen = datalen;

    memcpy_s(msg->data, datalen, buf, datalen);                                        //拷贝消息内容
    memcpy_s(msg->data + datalen, sizeof(msgidentfy), msgidentfy, sizeof(msgidentfy)); //加入消息尾标识

    if (!_blockmode) //如果为非阻塞模式，则直接返回，等后续socket可写时发送。
    {
        return datalen;
    }
    return Send();
}

//

bool tlink::Parse(ptxmsg *ppMsg, size_t &len)
{
    size_t datalen = &_recvbuf[0] + _recvbufpos - _revdata;
    bool bRet = Parse(&_revdata, datalen, ppMsg, len);
    return bRet;
}

bool tlink::Parse(char **ppPuf, size_t datalen, ptxmsg *ppMsg, size_t &len)
{
    size_t pos = 0;
    char *bpos = *ppPuf;
    
    txmsg *pMsg = nullptr;

    while (pos + sizeof(msgidentfy) <= datalen)
    {
        int ret = memcmp(bpos + pos, msgidentfy, sizeof(msgidentfy));
        if (ret == 0) //找到消息结束标识符
        {
            _recvmsgcount++;
            len = bpos + pos - *ppPuf;
            pMsg = (txmsg *)*ppPuf;
            pos += sizeof(msgidentfy);
            *ppPuf += pos;
            break; //每次只收一个消息.
        }
        else
        {
            ++pos;
        }
    }

    if (pMsg != nullptr)
    {
        *ppMsg = pMsg;
        return true;
    }

    return false;
}

int tlink::Recv()
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
            return -1;
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

    return recvcount;
}

int tlink::RecvMessage(ptxmsg *ppMsg, size_t &len)
{
    ptxmsg pMsg = nullptr;
    char recvbuf[recvbuflen] = {0};
    int recvcount = 0;
    char *precvbuf = &recvbuf[0];
    int waittimes = 0;
    do
    {
        int iResult = incInstance()->recvI(_socket, precvbuf + recvcount, recvbuflen, 0);
        if (iResult < 0)
        {
            int errcode = WSAGetLastError();
            if (errcode == WSAEWOULDBLOCK)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            return TXLNKRECVERR;
        }
        if (iResult == 0)
        {
            return TXLNKRECVERR;
        }
        recvcount += iResult;
        /* code */
        if (Parse(&precvbuf, recvcount, &pMsg, len))
        {
            *ppMsg = txmsg::Clone(pMsg);
            return true;
        }
    } while (waittimes++ < 100);

    return TXLNKRECVTO;
}

void tlink::HandleError()
{
}
