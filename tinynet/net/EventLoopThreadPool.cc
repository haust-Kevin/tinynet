/*
 * @Date: 2021-02-23 23:02:47
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-23 23:27:32
 * @FilePath: /tinynet/tinynet/net/EventLoopThreadPool.cc
 */

#include "tinynet/net/EventLoopThreadPool.h"

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop, const string &name)
    : _loop(loop), _name(name), _started(false), _threadNum(0), _next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb)
{
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
}

EventLoop *EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops()
{
}
