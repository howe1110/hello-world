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
