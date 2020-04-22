#include "tx_worker.h"
#include "tx_env.h"

worker_id_type tx_worker_base::_max_wkid = 0;
std::map<worker_id_type, tx_worker_base *> tx_worker_base::_worker_table;
std::vector<tx_timer_rec *> tx_worker_base::_timer_table(INVALID_TIMER, nullptr);

tx_worker_base::tx_worker_base(std::string n) : _name(n)
{
    _wkid = _max_wkid++;
    _isruning.store(false);
    _startswitch.store(false);
    _worker_table[_wkid] = this;
}

tx_worker_base::~tx_worker_base()
{
    std::map<worker_id_type, tx_worker_base *>::iterator pos = _worker_table.find(_wkid);
    if (pos == _worker_table.end())
    {
        /* code */
        return;
    }
    _worker_table.erase(_wkid);
}

void tx_worker_base::operator()()
{
    logwarn("tx_worker %s started.", _name.c_str());
    _isruning.store(true);
    proc();
}

void tx_worker_base::start()
{
    if (_startswitch.load()) //已经启动了
    {
        logerr("tx_worker %s allready started.", _name.c_str());
        return;
    }
    _startswitch.store(true);
    _t = std::thread(std::ref(*this));
    logwarn("tx_worker %s started.", _name.c_str());
}

void tx_worker_base::stop()
{
    if (!_startswitch.load())
    {
        return;
    }
    _startswitch.store(false);
    _t.join();
    _isruning.store(false);
}

tx_worker_base * tx_worker_base::GetWorkerById(worker_id_type id)
{
   auto pos = _worker_table.find(id);
   if (pos == _worker_table.end())
   {
       /* code */
       return nullptr;
   }
   return pos->second;
}

tx_worker::tx_worker(std::string n) : tx_worker_base(n)
{
}

tx_worker::~tx_worker()
{
}

void tx_worker::proc()
{
    while (startswitch())
    {
        txmsgptr msg;
        if (_msgqueue.read(msg, 1))
        {
            if (!msg.isNullPtr())
            {
                handleMessage(msg);
            }
        }
    }
}

void tx_worker::postmessage(txmsgptr pMsg)
{
    _msgqueue.write(pMsg);
}

void tx_worker::SendMessage(worker_id_type id, txmsgptr pMsg)
{
    std::map<worker_id_type, tx_worker_base *>::iterator pos = _worker_table.find(id);
    if (pos == _worker_table.end())
    {
        /* code */
        return;
    }
    tx_worker *wk = dynamic_cast<tx_worker *>(pos->second);
    if (wk)
    {
        wk->postmessage(pMsg);
    }
}

int tx_worker::regtimer(int tm, void *para)
{
    int i = 0;
    for (; i < INVALID_TIMER; ++i)
    {
        if (_timer_table[i])
        {
            continue;
        }
        _timer_table[i] = new tx_timer_rec(_wkid, i, tm, para);
        break;
    }
    return i;
}

void tx_worker::unregtimer(int id)
{
    if (_timer_table[id])
    {
        delete _timer_table[id];
        _timer_table[id] = nullptr;
    }
}
