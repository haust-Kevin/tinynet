/*
 * @Date: 2021-02-21 10:09:54
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 17:17:55
 * @FilePath: /tinynet/tinynet/net/EventLoop.cc
 */

#include "tinynet/net/EventLoop.h"
#include "tinynet/net/Channel.h"
#include "tinynet/net/Poller.h"
#include "tinynet/net/SocketsOps.h"

#include <algorithm>

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

namespace
{
    thread_local EventLoop *t_loopInThisThread = 0;
    const int __PollTimeMs = 10000;
    int createEventFd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        assert(evtfd >= 0);
        return evtfd;
    }

    struct IgnoreSigpipe
    {
        IgnoreSigpipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    } ignoreSigpipeObj;
}

EventLoop::EventLoop()
    : _looping(false),
      _quit(false),
      _eventHandling(false),
      _callingPendingFunctors(false),
      _iteration(0),
      _tid(this_thread::get_id()),
      _poller(Poller::newDefaultPoller(this)),
      // timeQueue(new TimeQueue(this)),
      _wakeupFd(createEventFd()),
      _wakeupChannel(new Channel(this, _wakeupFd)),
      _currentActiveChannel(nullptr)

{
    assert(!t_loopInThisThread);
    t_loopInThisThread = this;
    _wakeupChannel->setReadCallback(bind(&EventLoop::handleRead, this));
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
    _wakeupChannel->disableAll();
    _wakeupChannel->remove();
    ::close(_wakeupFd);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!_looping);
    assertInLoopThread();
    _quit = false;
    _looping = true;
    while (!_quit)
    {
        _activeChannels.clear();
        _pollReturnTime = _poller->poll(__PollTimeMs, &_activeChannels);
        ++_iteration;
        _eventHandling = true;
        for (Channel *channel : _activeChannels)
        {
            _currentActiveChannel = channel;
            _currentActiveChannel->handleEvent(_pollReturnTime);
        }
        _currentActiveChannel = nullptr;
        _eventHandling = false;
        doPendingFunctors();
    }
    _looping = false;
}

void EventLoop::quit()
{
    _quit = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    _poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (_eventHandling)
    {
        assert(_currentActiveChannel == channel ||
               find(_activeChannels.begin(), _activeChannels.end(), channel) == _activeChannels.end());
    }
    _poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return _poller->hasChannel(channel);
}

void EventLoop::runInLoop(Functor func)
{
    if (isInLoopThread())
    {
        func();
    }
    else
    {
        queueInLoop(func);
    }
}

void EventLoop::queueInLoop(Functor func)
{
    {
        lock_guard<mutex> guard(_mtx);
        _pendingFunctors.push_back(func);
    }
    if (!isInLoopThread() || _callingPendingFunctors)
    {
        wakeup();
    }
}

size_t EventLoop::queueSize() const
{
    lock_guard<mutex> guard(_mtx);
    return _pendingFunctors.size();
}

/*
 * *****************
 * // TODO 定时器任务
 * *****************
 */

bool EventLoop::isInLoopThread() const
{
    return _tid == this_thread::get_id();
}

void EventLoop::assertInLoopThread() const
{
    assert(isInLoopThread());
}

EventLoop *EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    size_t n = sockets::write(_wakeupFd, &one, sizeof one);
    assert(n == sizeof one);
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    size_t n = sockets::read(_wakeupFd, &one, sizeof one);
    assert(n == sizeof one);
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    _callingPendingFunctors = true;
    {
        lock_guard<mutex> guard(_mtx);
        functors.swap(_pendingFunctors);
    }
    for (auto func : functors)
    {
        func();
    }
    _callingPendingFunctors = false;
}