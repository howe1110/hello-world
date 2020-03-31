#include "tx_worker.h"

worker_id_type tx_worker_base::_max_wkid = 0;

tx_worker_base::tx_worker_base(std::string n) : _name(n)
{
    _wkid = _max_wkid++;
    _isruning.store(false);
    _startswitch.store(false);
}

tx_worker_base::~tx_worker_base()
{
}

void tx_worker_base::operator()()
{
    _isruning.store(true);
    proc();
}

void tx_worker_base::start()
{
    if(_startswitch.load())//已经启动了
    {
        return;
    }
    _startswitch.store(true);
    _t = std::thread(std::ref(*this));
}

void tx_worker_base::stop()
{
    if(!_startswitch.load())
    {
        return;
    }
    _startswitch.store(false);
    _t.join();
    _isruning.store(false);
}

std::map<worker_id_type, tx_worker *> tx_worker::_worker_table;

tx_worker::tx_worker(std::string n):tx_worker_base(n)
{
    _worker_table[_wkid] = this;
}

tx_worker::~tx_worker()
{
    std::map<worker_id_type, tx_worker *>::iterator pos = _worker_table.find(_wkid);
    if (pos == _worker_table.end())
    {
        /* code */
        return;
    }
    _worker_table.erase(_wkid);
}

void tx_worker::proc()
{
    Trace("tx_worker {%s} started.", _name.c_str());
    while (startswitch())
    {
        txmsgptr msg;
        if(_msgqueue.read(msg, 1))
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
    std::map<worker_id_type, tx_worker *>::iterator pos = _worker_table.find(id);
    if (pos == _worker_table.end())
    {
        /* code */
        return;
    }
    pos->second->postmessage(pMsg);    
}



