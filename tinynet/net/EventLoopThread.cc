/*
 * @Date: 2021-02-23 22:27:59
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-23 23:27:47
 * @FilePath: /tinynet/tinynet/net/EventLoopThread.cc
 */

#include "tinynet/net/EventLoopThread.h"
#include "tinynet/net/EventLoop.h"

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const string &name)
    : _loop(nullptr),
      _exiting(false),
      _thread(bind(&EventLoopThread::threadFunc, this), name),
      _callback(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    _exiting = true;
    if (_loop)
    {
        _loop->quit();
        _thread.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    assert(!_thread.started());
    _thread.start();
    unique_lock<mutex> guard(_mtx);
    _cv.wait(guard, [this]() { return _loop != nullptr; });
    return _loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    if (_callback)
    {
        _callback(&loop);
    }
    {
        lock_guard<mutex> guard(_mtx);
        _loop = &loop;
        _cv.notify_one();
    }
    loop.loop();
    {
        lock_guard<mutex> guard(_mtx);
        _loop = nullptr;
    }
}