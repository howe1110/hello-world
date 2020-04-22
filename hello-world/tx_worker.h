#pragma once

#include <string>
#include <map>
#include <atomic>
#include <thread>
#include "CommonDef.h"
#include "tx_base.h"
#include "tx_queue.h"
#include "tx_msg.h"

typedef tx_queue<txmsg> msg_queue;

typedef int worker_type;
typedef size_t worker_id_type;
typedef unsigned int timer_id_type;
typedef long long timer_tick_type;

const size_t INVALID_TIMER = 1024;

struct tx_timer_rec
{
  tx_timer_rec(worker_id_type wkid, int id, int tm, void *p)
  {
    _wkid = wkid;
    _id = id;
    _timeout = tm;
    _para = p;
    _tick = 0;
  }
  ~tx_timer_rec() = default;
  int _id;      //定时器ID
  int _timeout; //超时时间
  void *_para;  //参数
  worker_id_type _wkid;
  timer_tick_type _tick;
};

class tx_worker_base : public tx_base
{
protected:
  static std::map<worker_id_type, tx_worker_base *> _worker_table;
  static std::vector<tx_timer_rec *> _timer_table;

protected:
  std::atomic<bool> _isruning;    //运行状态
  std::atomic<bool> _startswitch; //启动开关
  std::thread _t;
  worker_id_type _wkid;

protected:
  static worker_id_type _max_wkid;
  static timer_id_type _max_tmid;

protected:
  /* data */
  std::string _name;

public:
  tx_worker_base(std::string n);
  virtual ~tx_worker_base();

public:
  bool isrunning() { return _isruning.load(); }
  bool startswitch() { return _startswitch.load(); }
  worker_id_type getid() { return _wkid; }
  tx_worker_base *GetWorkerById(worker_id_type id);

public:
  void operator()();
  virtual void proc() = 0;

public:
  void start();
  virtual void stop();
};

class tx_worker : public tx_worker_base
{
private:
  /* data */
  tx_queue<txmsgptr> _msgqueue; //接收队列

public:
  tx_worker(std::string n);
  virtual ~tx_worker();

public:
  void proc();

public:
  virtual void handleMessage(txmsgptr pMsg) = 0;
  void postmessage(txmsgptr pMsg);
  void SendMessage(worker_id_type id, txmsgptr pMsg);

protected:
  int regtimer(int tm, void *para);
  void unregtimer(int id);
};
