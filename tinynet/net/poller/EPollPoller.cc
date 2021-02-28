/*
 * @Date: 2021-02-25 17:20:57
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-28 11:50:37
 * @FilePath: /tinynet/tinynet/net/poller/EPollPoller.cc
 */

#include "tinynet/net/poller/EPollPoller.h"
#include "tinynet/net/Channel.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

namespace
{
    const int __New = -1;
    const int __Added = 1;
    const int __Deleted = 2;
} // namespace

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
      _epollfd(::epoll_create1(EPOLL_CLOEXEC)),
      _events(__InitEventListSize)
{
    assert(_epollfd >= 0);
}

EPollPoller::~EPollPoller()
{
    ::close(_epollfd);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    int eventsNum = ::epoll_wait(_epollfd,
                                 &*_events.begin(),
                                 static_cast<int>(_events.size()),
                                 timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (eventsNum > 0)
    {
        fillActiveChannels(eventsNum, activeChannels);
        if (eventsNum == _events.size())
        {
            _events.resize(_events.size() * 2);
        }
    }
    else if (eventsNum == 0)
    {
    }
    else
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            // TODO log
        }
    }
    return now;
}

void EPollPoller::updateChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    const int index = channel->index();
    if (index == __New || index == __Deleted)
    {
        int fd = channel->fd();
        if (index == __New)
        {
            assert(_channels.find(fd) == _channels.end());
            _channels[fd] = channel;
        }
        else
        {
            assert(_channels.find(fd) != _channels.end());
            assert(_channels[fd] == channel);
        }
        channel->setIndex(__Added);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        assert(_channels.find(fd) != _channels.end());
        assert(_channels[fd] == channel);
        assert(index == __Added);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(__Deleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    int fd = channel->fd();
    assert(_channels.find(fd) != _channels.end());
    assert(_channels[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == __Added || index == __Deleted);
    size_t n = _channels.erase(fd);
    assert(n == 1);
    if (index == __Added)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(__New);
}

void EPollPoller::fillActiveChannels(int eventNum, ChannelList *activeChannels) const
{
    assert(eventNum <= _events.size());
    for (int i = 0; i < eventNum; i++)
    {
        Channel *channel = static_cast<Channel *>(_events[i].data.ptr);
        // int fd = channel->fd();
        // ChannelMap::const_iterator iter = _channels.find(fd);
        // assert(iter != _channels.end());
        // assert(iter->second == channel);
        channel->setREvents(_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel *channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(_epollfd, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            // TODO: log
        }
        else
        {
            // TODO log
            assert(0);
        }
    }
}
