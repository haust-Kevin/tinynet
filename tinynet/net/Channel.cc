/*
 * @Date: 2021-02-21 20:22:16
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-22 17:40:48
 * @FilePath: /tinynet/tinynet/net/Channel.cc
 */

#include "tinynet/net/Channel.h"
#include "tinynet/base/utils.h"
#include "tinynet/net/EventLoop.h"

#include <poll.h>

using namespace tinynet;
using namespace tinynet::net;

const int Channel::__NoneEvent = 0;
const int Channel::__ReadEvent = POLLIN | POLLPRI;
const int Channel::__WriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : _loop(loop), _fd(fd), _events(__NoneEvent), _revents(__NoneEvent),
      _index(-1), _tied(false), _eventHandling(false), _addedToLoop(false)
{
}

Channel::~Channel()
{
    assert(!_eventHandling && !_addedToLoop);
    if (_loop->isInLoopThread())
    {
        assert(!_loop->hasChannel(this));
    }
}

void Channel::handleEvent(Timestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (_tied)
    {
        guard = _tie.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    _eventHandling = true;
    if ((_revents & POLLHUP) && !(_revents & POLLIN) && _closeCb)
    {
        _closeCb();
    }

    if (_revents & (POLLERR | POLLNVAL) && _errorCb)
    {
        _errorCb();
    }

    if (_revents & (POLLIN | POLLPRI | POLLRDHUP) && _readCb)
    {
        _readCb(receiveTime);
    }

    if (_revents & POLLOUT && _writeCb)
    {
        _writeCb();
    }
    _eventHandling = false;
}

void Channel::enableReading()
{
    _events |= __ReadEvent;
    update();
}

void Channel::disableReading()
{
    _events &= ~__ReadEvent;
    update();
}

void Channel::enableWriting()
{
    _events |= __WriteEvent;
    update();
}

void Channel::disableWriting()
{
    _events &= ~__WriteEvent;
    update();
}

void Channel::disableAll()
{
    _events = __NoneEvent;
    update();
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    _tie = obj;
    _tied = true;
}

void Channel::remove()
{
    assert(isNoneEvent());
    _addedToLoop = false;
    _loop->removeChannel(this);
}

void Channel::update()
{
    _addedToLoop = true;
    _loop->updateChannel(this);
}