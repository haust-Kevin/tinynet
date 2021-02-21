/*
 * @Date: 2021-02-21 20:22:07
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-21 21:39:44
 * @FilePath: /tinynet/tinynet/net/Channel.h
 */

#ifndef TINYNET_NET_CHANNEL_H
#define TINYNET_NET_CHANNEL_H

#include "tinynet/base/Timestamp.h"
#include "tinynet/base/utils.h"

#include <functional>

namespace tinynet
{
    namespace net
    {
        class Channel : noncopyable
        {
        private:
            static const int __NoneEvent;
            static const int __ReadEvent;
            static const int __WriteEvent;

        public:
            using EventCallback = std::function<void()>;
            using ReadEventCallback = std::function<void(Timestamp)>;

            Channel(EventLoop *loop, int fd);
            ~Channel();

            void handleEvent(Timestamp receiveTime);

            void setReadCallback(ReadEventCallback cb) { _readCb = cb; }
            void setWriteCallback(EventCallback cb) { _writeCb = cb; }
            void setCloseCallback(EventCallback cb) { _closeCb = cb; }
            void setErrorCallback(EventCallback cb) { _errorCb = cb; }

            void tie(const std::shared_ptr<void> &obj);

            int fd() const { return _fd; }
            int events() { return _events; }
            void setREvents(int revent) { _revents = revent; }
            bool isNoneEvent() const {}

        private:
            void update();
            void handleEventWithGuard(Timestamp receiveTime);

        private:
            EventLoop *_loop;
            const int _fd;
            int _events;
            int _revents;
            int _index;
            // bool logHup;
            std::weak_ptr<void> _tie;
            bool _tied;
            bool _eventHandling;
            bool _addedToLoop;

            ReadEventCallback _readCb;
            EventCallback _writeCb;
            EventCallback _closeCb;
            EventCallback errorCb;
        };
    }
}

#endif // !TINYNET_NET_CHANNEL_H