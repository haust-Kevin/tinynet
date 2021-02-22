/*
 * @Date: 2021-02-21 10:09:54
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-22 20:20:09
 * @FilePath: /tinynet/tinynet/net/EventLoop.cc
 */

#include "tinynet/net/EventLoop.h"
#include "tinynet/net/Channel.h"
#include "tinynet/net/Poller.h"

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

namespace
{
    thread_local EventLoop *t_loopInThisThread = 0;
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
}

void EventLoop::quit()
{
}

void EventLoop::wakeup()
{
}

void EventLoop::updateChannel(Channel *channel)
{
}

void EventLoop::removeChannel(Channel *channel)
{
}

bool EventLoop::hasChannel(Channel *channel)
{
    //
    return 0;
}

void EventLoop::runInLoop(Functor func)
{
}

void EventLoop::queueInLoop(Functor func)
{
}

size_t EventLoop::queueSize() const
{
    //
    return 0;
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

void EventLoop::handleRead()
{
}

void EventLoop::doPendingFunctorsd()
{
}