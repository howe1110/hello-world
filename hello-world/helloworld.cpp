#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>

#include "BNode.h"
#include "tcpcomm.h"

#include <semaphore.h>
#include <windows.h>

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
    BNode::instance()->Show();
}

void TraceNodeDFunc(const std::vector<std::string> &paras)
{
    BNode::instance()->StartTrace();
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
    BNode::instance()->StartJoin(paras[0], paras[1]);
}

void checkFunc(const std::vector<std::string> &paras)
{
    BNode::instance()->stabilization();
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
    network* nw = new tcpcomm();
    if(nw == nullptr)
    {
        std::cout << "initialize incinstance failed." << std::endl;
        return false;
    }

    SetincInstance(nw);

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

int main()
{
    initialize();

    BNode::instance()->Start();
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
    return 0;
}
