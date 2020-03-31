#pragma once

#include <string>
#include <map>
#include <atomic>

#include "tx_base.h"
#include "tx_queue.h"
#include "tx_msg.h"
class CNode;
typedef tx_queue<txmsg> msg_queue;

typedef int worker_type;
typedef size_t worker_id_type;

class tx_worker_base : public tx_base
{
protected:
  std::atomic<bool> _isruning;//运行状态
  std::atomic<bool> _startswitch;//启动开关
  std::thread _t;
  worker_id_type _wkid;

protected:
  static worker_id_type _max_wkid;

protected:
  /* data */
  std::string _name;

public:
  tx_worker_base(std::string n);
  virtual ~tx_worker_base();

public:
  bool isrunning() { return _isruning.load(); }
  bool startswitch() { return _startswitch.load(); }
  worker_id_type getid(){return _wkid;}
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
  static std::map<worker_id_type, tx_worker *> _worker_table;
  static std::map<worker_type, tx_worker *> _worker_sub_table;

public:
  tx_worker(std::string n);
  virtual ~tx_worker();

public:
  void proc();

public:
  virtual void handleMessage(txmsgptr pMsg) = 0;
  void postmessage(txmsgptr pMsg);
  void SendMessage(worker_id_type id, txmsgptr pMsg);
};
