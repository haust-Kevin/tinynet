/*
 * @Date: 2021-02-25 17:21:14
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-26 13:04:11
 * @FilePath: /tinynet/tinynet/net/poller/EPollPoller.h
 */

#ifndef TINYNET_NET_EPOLLPOLLER
#define TINYNET_NET_EPOLLPOLLER

#include "tinynet/net/Poller.h"

struct epoll_event;

namespace tinynet
{
    namespace net
    {

        class EPollPoller : public Poller
        {
            using EventList = std::vector<struct epoll_event>;
            static const int __InitEventListSize = 16;

        public:
            EPollPoller(EventLoop *loop);
            ~EPollPoller();

            Timestamp poll(int timeoutMs, ChannelList *activeChannels);
            void updateChannel(Channel *channel);
            void removeChannel(Channel *channel);

        private:
            void fillActiveChannels(int eventNum, ChannelList *activeChannels) const;
            void update(int operation, Channel *channel);

        private:
            int _epollfd;
            EventList _events;
        };
    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_EPOLLPOLLER