#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include "tcpcomm.h"
#include "tx_worker.h"
#include <semaphore.h>
#include "StarNode.h"
#include "tx_env.h"
#include "devinf.h"

std::string promot = ">";

const std::string guidenodeip = "192.168.1.81";
const std::string guidenodeport = "27015";

void strsplit(const std::string &s, std::string &funcname, std::vector<std::string> &paras)
{
    std::istringstream iss(s);
    std::string ss;
    int argc = 0;
    while (std::getline(iss, ss, ' '))
    {
        if (argc++ == 0)
        {
            funcname = ss;
            continue;
        }
        paras.push_back(ss);
    }
}

typedef void (*pfunc)(const std::vector<std::string> &paras);
std::map<std::string, pfunc> funcmap;

void proc(const std::string &line)
{
    std::vector<std::string> paras;
    std::string funcname;
    strsplit(line, funcname, paras);
    if (funcname.empty())
    {
        return;
    }
    std::map<std::string, pfunc>::iterator pos = funcmap.find(funcname);
    if (pos == funcmap.end())
    {
        std::cout << "invalid command:" << funcname << std::endl;
        return;
    }

    pfunc func = pos->second;

    func(paras);
}

void dispNodeDFunc(const std::vector<std::string> &paras)
{
}

void TraceNodeDFunc(const std::vector<std::string> &paras)
{
}

void dispHelp(const std::vector<std::string> &paras)
{
    for (std::map<std::string, pfunc>::iterator it = funcmap.begin(); it != funcmap.end(); ++it)
    {
        std::cout << it->first << std::endl;
    }
}

void joinFunc(const std::vector<std::string> &paras)
{
    if (paras.size() != 2)
    {
        std::cout << "Invalid parameters." << std::endl;
        return;
    }
}

void checkFunc(const std::vector<std::string> &paras)
{
}

void startFunc(const std::vector<std::string> &paras)
{
}

sem_t app_sem;

bool initialize()
{
    int err = sem_init(&app_sem, 0, 1);
    if (err != 0)
    {
        std::cout << "initialize semaphore failed." << std::endl;
        return false;
    }

    funcmap["shownode"] = dispNodeDFunc;
    funcmap["trace"] = TraceNodeDFunc;
    funcmap["??"] = dispHelp;
    funcmap["join"] = joinFunc;
    funcmap["check"] = checkFunc;

    return true;
}

void release()
{
    sem_destroy(&app_sem);
}

class tx_worker_test : public tx_worker
{
private:
public:
    tx_worker_test(std::string n) : tx_worker(n) {}
    ~tx_worker_test() {}

public:
    void handleMessage(txmsgptr pMsg)
    {
        std::cout << "handle message." << std::endl;
    }
};

void startshell()
{
    initialize();

    std::cout << promot;
    std::vector<std::string> paras;
    for (std::string line; std::getline(std::cin, line);)
    {
        if (line.compare("quit") == 0)
        {
            break;
        }
        proc(line);
        std::cout << promot;
    }
    release();
}

bool InitNetwork()
{
    tcpcomm *pcomm = new tcpcomm();
    if (pcomm == nullptr)
    {
        logerr("New network instance failed.");
        return false;
    }
    SetincInstance(pcomm);
    return true;
}

void RestNetwork()
{
    ResetincInstance();
}

void start()
{
    StarNode::instance().Start();
}

int main()
{
    if (-1 == daemon(1, 1))
    {
        logerr("Failed daemonizing");
        return 1;
    }
    logerr("tx service is starting.");
    InitNetwork();
    start();
    return 0;
}
