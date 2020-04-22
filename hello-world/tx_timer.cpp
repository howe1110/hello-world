#include "tx_timer.h"
#include <chrono>
#include <condition_variable>

tx_timer::tx_timer(/* args */) : tx_worker_base("tx_timer")
{
}

tx_timer::~tx_timer()
{
}

void tx_timer::proc()
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable cv;
    while (startswitch())
    {
        if(cv.wait_for(lck, std::chrono::milliseconds(10)) == std::cv_status::timeout)
        {
            handletimer();
        }
    }
}

void tx_timer::handletimer()
{
    for (auto i = 0; i < _timer_table.size(); i++)
    {
        if (_timer_table[i] == nullptr)
        {
            continue;
        }

        tx_worker_base *p = GetWorkerById(_timer_table[i]->_wkid);
        if (p == nullptr)
        {
            continue;
        }

        tx_worker *tw = dynamic_cast<tx_worker *>(p);
        if (tw == nullptr)
        {
            continue;
        }
        _timer_table[i]->_tick++;
        if ((_timer_table[i]->_tick*10 % _timer_table[i]->_timeout) == 0)
        {
            timerevent tem(_timer_table[i]->_id, _timer_table[i]->_timeout, _timer_table[i]->_para);
            txmsg *pm = txmsg::Create(&tem, msgid_timerevent, sizeof(timerevent));
            tw->postmessage(txmsgptr(pm));
        }
    }
}

txmsgptr tx_timer::createtimermsg()
{
}