/*
 * @Date: 2021-02-19 16:51:49
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-22 20:21:30
 * @FilePath: /tinynet/tinynet/net/Poller.h
 */

#ifndef TINYNET_NET_POLLER_H
#define TINYNET_NET_POLLER_H

#include "tinynet/base/Timestamp.h"
#include "tinynet/base/utils.h"
// #include "tinynet/net/EventLoop.h"

#include <map>
#include <vector>

namespace tinynet
{
    namespace net
    {
        class Channel;
        class EventLoop;

        class Poller : noncopyable
        {
        public:
            using ChannelList = std::vector<Channel *>;
            Poller(EventLoop *loop);
            virtual ~Poller() = default;

            virtual bool hasChannel(Channel *channel) const;

            virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
            virtual void upadteChannel(Channel *channel) = 0;
            virtual void removeChannel(Channel *channel) = 0;

            void assertInLoopThread() const
            {
                _ownerLoop->assertInLoopThread();
            }
            static Poller *newDefaultPoller(EventLoop *loop);

        protected:
            using ChannelMap = std::map<int, Channel *>;
            ChannelMap _channels;

        private:
            EventLoop *_ownerLoop;
        };
    }
}

#endif // !TINYNET_NET_POLLER_H