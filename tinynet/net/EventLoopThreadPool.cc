/*
 * @Date: 2021-02-23 23:02:47
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-26 13:11:46
 * @FilePath: /tinynet/tinynet/net/EventLoopThreadPool.cc
 */

#include "tinynet/net/EventLoopThreadPool.h"
#include "tinynet/net/EventLoop.h"
#include "tinynet/net/EventLoopThread.h"

#include <sstream>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop, const string &name)
    : _mainLoop(loop), _name(name), _started(false), _threadNum(0), _next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb)
{
    assert(!_started);
    _mainLoop->assertInLoopThread();
    _started = true;
    for (int i = 0; i < _threadNum; i++)
    {
        stringstream sos;
        sos << _name.c_str() << i;
        _threads.push_back(make_unique<EventLoopThread>(cb, sos.str()));
        _loops.push_back(_threads[i]->startLoop());
    }
    if (!_threadNum && cb)
    {
        cb(_mainLoop);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    _mainLoop->assertInLoopThread();
    assert(_started);
    EventLoop *loop = _mainLoop;
    if (!_loops.empty())
    {
        loop = _loops[_next];
        _next++;
        if (_next >= _loops.size())
        {
            _next = 0;
        }
    }
    return loop;
}

EventLoop *EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    _mainLoop->assertInLoopThread();
    EventLoop *loop = _mainLoop;
    if (!_loops.empty())
    {
        loop = _loops[hashCode % _loops.size()];
    }
    return loop;
}

vector<EventLoop *> EventLoopThreadPool::getAllLoops()
{
    _mainLoop->assertInLoopThread();
    assert(_started);
    if (!_loops.empty())
    {
        return vector<EventLoop *>(1, _mainLoop);
    }
    return _loops;
}
