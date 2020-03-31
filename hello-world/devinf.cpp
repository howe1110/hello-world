#include "devinf.h"

#include <iostream>

devinf* nw = nullptr;

void SetincInstance(devinf* p)
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

devinf* incInstance()
{
    if(nw == nullptr)
    {
        std::cout << "Device interface not set." << std::endl;
        return nullptr;
    }
    return nw;
}