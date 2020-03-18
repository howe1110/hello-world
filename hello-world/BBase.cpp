#include "BBase.h"
#include <stdio.h>
#include <stdarg.h>


network* nw = nullptr;

void SetincInstance(network* p)
{
    nw = p;
}

void ResetincInstance()
{
    if(nw != nullptr)
    {
        delete nw;
        nw = nullptr;
    }
}

network* incInstance()
{
    if(nw == nullptr)
    {
        printf("Error, incIntance net set.\r\n");
        return nullptr;
    }
    return nw;
}

BBase::BBase(/* args */) : _traceswitch(true)
{
}

BBase::~BBase()
{
}

void BBase::strsplit(const std::string &s, std::string &funcname, std::vector<std::string> &paras)
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

void BBase::strsplit(const std::string &s, const char delim, std::vector<std::string> &paras)
{
    std::istringstream iss(s);
    std::string ss;
    int argc = 0;
    while (std::getline(iss, ss, delim))
    {
        paras.push_back(ss);
    }
}

void BBase::StartTrace()
{
    _traceswitch = true;
}

void BBase::StopTrace()
{
    _traceswitch = false;
}

void BBase::Trace(const char *format, ...)
{
    if (!_traceswitch)
    {
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    fprintf(stdout, "\n");
}
