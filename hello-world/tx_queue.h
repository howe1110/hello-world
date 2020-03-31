#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
template <class T>
class tx_queue
{
    typedef bool (tx_queue<T>::*preFunc)();

public:
    tx_queue(){};
    virtual ~tx_queue(){};

public:
    void write(T e)
    {
        std::unique_lock<std::mutex> lk(_mtx);
        _queue.push(e);
        _cv.notify_all();
    };

    bool read(T &e, unsigned int sec)
    {
        std::unique_lock<std::mutex> lck(_mtx);
        if (!_cv.wait_for(lck, std::chrono::seconds(sec), std::bind(&tx_queue<T>::notEmpty, this)))
        {
            return false;
        }
        e = _queue.front();
        _queue.pop();
        return true;
    };
    
    bool read(T &e)
    {
        std::unique_lock<std::mutex> lck(_mtx);
        if(-_queue.size() <= 0)
        {
            return false;
        }
        e = _queue.front();
        _queue.pop();
        return true;
    };

    size_t size()
    {
        std::unique_lock<std::mutex> lk(_mtx);
        return _queue.size();
    };

    bool notEmpty()
    {
        return _queue.size() > 0;
    };

private:
    std::queue<T> _queue;
    std::mutex _mtx;
    std::condition_variable _cv;
};