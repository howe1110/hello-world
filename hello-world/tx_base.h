#pragma once

#include <functional>
#include <memory>

class tx_base
{
private:
    /* data */
public:
    tx_base(/* args */);
    ~tx_base();

public:

public:
    void StartTrace();
    void StopTrace();

public:
    void Trace(const char *format, ...);

private:
    bool _traceswitch;
};
