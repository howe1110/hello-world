#include "appwork.h"

appwork::appwork(/* args */)
{
}

appwork::~appwork()
{
}

void appwork::proc()
{
    while (isrunning())
    {
        txmsgptr msg;
        _msgqueue.read(msg, 1);
        if (!msg.isNullPtr())
        {
            handleMessage(msg);
        }
    }
}

void appwork::postmessage(txmsgptr pMsg)
{
    _msgqueue.write(pMsg);
}

