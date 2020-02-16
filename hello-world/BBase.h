
#pragma once

#include <string>
#include <map>
#include <sstream>
#include <vector>

class BBase
{
private:
    /* data */
public:
    BBase(/* args */);
    ~BBase();

public:
    static void strsplit(const std::string &s, const char delim, std::vector<std::string> &paras);
    static void strsplit(const std::string &s, std::string &funcname, std::vector<std::string> &paras);

public:
    void StartTrace();
    void StopTrace();

public:
    void Trace(const char *format, ...);

private:
    bool _traceswitch;
};
