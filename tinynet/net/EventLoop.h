/*
 * @Date: 2021-02-21 10:10:00
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-21 11:47:33
 * @FilePath: /tinynet/tinynet/net/EventLoop.h
 */

#ifndef TINYNET_NET_EVENTLOOP_H
#define TINYNET_NET_EVENTLOOP_H

#include "tinynet/base/utils.h"

namespace tinynet
{
    namespace net
    {

        class EventLoop : noncopyable
        {
        public:
            void assertInLoopThread() const;
        };
    }
}

#endif // !TINYNET_NET_EVENTLOOP_H