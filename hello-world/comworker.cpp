#include "comworker.h"
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

void comworker::PostSocket(SOCKET st)
{
    _socketqueue.write(st);
}

void comworker::proclinkqueue()
{
    bool result = true;
    
    while (result)
    {
        SOCKET st = INVALID_SOCKET;
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

FD_SET comworker::getFdSet()
{
    FD_SET fds_;
    FD_ZERO(&fds_);
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        FD_SET(it->second->GetSocket(), &fds_);
    }
    return fds_;
}

FD_SET comworker::getWriteSet()
{
    FD_SET fds_;
    FD_ZERO(&fds_);
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        if (it->second->SendBufSize() > 0 && it->second->GetState() == eConnected)
        {
            FD_SET(it->second->GetSocket(), &fds_);
        }
    }
    return fds_;
}

void comworker::handleReadSockets(FD_SET fds)
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

void comworker::handleWriteSocket(FD_SET fds)
{
    for (std::map<size_t, tlinkptr>::iterator it = _socketmap.begin(); it != _socketmap.end(); ++it)
    {
        if (FD_ISSET(it->second->GetSocket(), &fds) > 0)
        {
            it->second->Send();
        }
    }
}

void comworker::handleErrorSocket(FD_SET fds)
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
    TIMEVAL tval;
    tval.tv_sec = 1;
    tval.tv_usec = 0;
    int ret = 0;

    while (startswitch())
    {
        proclinkqueue();
        FD_SET fds_r = getFdSet();
        FD_SET fds_w = getWriteSet();
        FD_SET fds_e = getFdSet();
        ret = incInstance()->selectI(0, &fds_r, (fd_set *)&fds_w, (fd_set *)&fds_e, &tval);
        if (ret > 0)
        {
            handleReadSockets(fds_r);
            handleWriteSocket(fds_w);
            handleErrorSocket(fds_e);
        }
    }
}
