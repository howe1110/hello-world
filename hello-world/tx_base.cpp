#include "tx_base.h"
#include <stdio.h>
#include <stdarg.h>

tx_base::tx_base(/* args */) : _traceswitch(true)
{
}

tx_base::~tx_base()
{
}

void tx_base::StartTrace()
{
    _traceswitch = true;
}

void tx_base::StopTrace()
{
    _traceswitch = false;
}

void tx_base::Trace(const char *format, ...)
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

