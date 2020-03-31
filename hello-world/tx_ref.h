#pragma once

#include <mutex>
#include <iostream>

template <typename T>
class txRefPtr;

template <typename T>
class txRef
{
private:
    txRef(T *ptr) : p(ptr), count(1){};
    ~txRef()
    {
        if(p != nullptr)
        {
            delete p;
        }
    };
    friend class txRefPtr<T>;
    int ref()
    {
        return ++count;
    };
    int unref()
    {
        return --count;
    };
    int getCount()
    {
        return count;
    };

private:
    
    int count; //引用计数
    T *p;      //基础对象指针
};

template <typename T>
class txRefPtr
{
public:
    txRefPtr():ref(nullptr){};
    txRefPtr(T *ptr) : ref(new txRef<T>(ptr)){};
    txRefPtr(const txRefPtr<T> &nref)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        ref = nref.ref;
        if(nref.ref == nullptr)
        {
            return;
        }
        ref->ref();
    };
    virtual ~txRefPtr()
    {
        std::lock_guard<std::mutex> lck(_mtx);
        if(ref == nullptr)
        {
            return;
        }

        if (ref->unref() <= 0)
        {
            delete ref;
        }
    };

    txRefPtr &operator=(const txRefPtr<T> &rhs)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        if (&rhs == this)
        {
            return *this;
        }
        if(ref != nullptr)
        {
            if (ref->unref() <= 0)
            {
                delete ref;
            }
        }

        ref = rhs.ref;
        ref->ref();

        return *this;
    };

    T &operator*()
    {
        return *(ref->p);
    };

    T *operator->()
    {
        return ref->p;
    };

    bool isNullPtr()
    {
        return (ref->p == nullptr);
    }

    inline int getRefCount() { return ref->getCount(); }

private:
    std::mutex _mtx;
    txRef<T> *ref;
};
