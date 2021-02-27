/*
 * @Date: 2021-02-22 19:42:55
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-26 10:59:44
 * @FilePath: /tinynet/tinynet/net/poller/DefaultPoller.cc
 */

#include "tinynet/net/Poller.h"
#include "tinynet/net/poller/EPollPoller.h"

using namespace tinynet::net;

Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    return new EPollPoller(loop);
}
