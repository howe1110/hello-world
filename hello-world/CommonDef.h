#pragma once
typedef unsigned int IDtype;
typedef unsigned int MSGIDTYPE;

enum msgtype
{
    msgtype_shakehand = 0,
    successor_req,
    successor_rsp,
    join_req,
    join_rsp,
    stabilize_req,
    stabilize_rsp
};


struct timerevent
{
    timerevent(int id, int tm, void *p)
    {
        _id = id;
        _timeout = tm;
        _para = p;
    }
    ~timerevent() = default;
    int _id;//定时器ID
    int _timeout;//超时时间
    void *_para;//参数
};

