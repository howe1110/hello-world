#include "comworker.h"
#include "devinf.h"

comworker::comworker(/* args */) : tx_worker_base("comworker")
{
}

comworker::~comworker()
{
}

void comworker::stop()
{
    close();
    tx_worker_base::stop();
}

void comworker::PostSocket(int st)
{
    _socketqueue.write(st);
}

void comworker::proclinkqueue()
{
    bool result = true;
    while (result)
    {
        int st = INVALID_SOCKET;
        result = _socketqueue.read(st);
        if (result)
        {
            tlinkptr lnk(new tlink(st, false));
            if (lnk.isNullPtr())
            {
                continue;
            }
            _socketmap[lnk->getId()] = lnk;
        }
    }
}

void comworker::getFdSet(fd_set& fds)
{
    FD_ZERO(&fds);
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        FD_SET(it->second->GetSocket(), &fds);
    }
}

void comworker::getWriteSet(fd_set& fds)
{
    FD_ZERO(&fds);
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        if (it->second->SendBufSize() > 0 && it->second->GetState() == eConnected)
        {
            FD_SET(it->second->GetSocket(), &fds);
        }
    }
}

void comworker::handleReadSockets(fd_set fds)
{
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end();)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            it->second->Recv();
            ptxmsg pMsg = nullptr;
            size_t len = 0;
            while (it->second->Parse(&pMsg, len))
            {
                /* code */
                handlemessage(it->second, pMsg);
            }
        }
        if (it->second->GetState() == eDisconnect)
        {
            _socketmap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void comworker::handleWriteSocket(fd_set fds)
{
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            it->second->Send();
        }
    }
}

void comworker::handleErrorSocket(fd_set fds)
{
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            it->second->HandleError();
        }
    }
}

void comworker::close()
{
    _socketmap.clear();
}

void comworker::proc()
{
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ret = 0;

    while (startswitch())
    {
        proclinkqueue();
        fd_set fds_r;
        getFdSet(fds_r);
        fd_set fds_w;
        getWriteSet(fds_w);
        fd_set fds_e;
        getFdSet(fds_e);
        ret = incInstance()->selectI(FD_SETSIZE, &fds_r, &fds_w, &fds_e, &tv);
        if (ret > 0)
        {
            handleReadSockets(fds_r);
            handleWriteSocket(fds_w);
            handleErrorSocket(fds_e);
        }
    }
}
