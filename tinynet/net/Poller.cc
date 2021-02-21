/*
 * @Date: 2021-02-21 10:06:26
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-21 22:35:22
 * @FilePath: /tinynet/tinynet/net/Poller.cc
 */

#include "tinynet/net/Poller.h"
#include "tinynet/net/Channel.h"

using namespace tinynet;
using namespace tinynet::net;

Poller::Poller(EventLoop *loop)
    : _ownerLoop(loop)
{
}

bool Poller::hasChannel(Channel *channel) const
{
    assertInLoopThread();
    ChannelMap::const_iterator iter = _channels.find(channel->fd());
    return iter != _channels.end() && iter->second == channel;
}
