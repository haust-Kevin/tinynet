/*
 * @Date: 2021-02-21 20:22:07
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-28 11:45:12
 * @FilePath: /tinynet/tinynet/net/Channel.h
 */

#ifndef TINYNET_NET_CHANNEL_H
#define TINYNET_NET_CHANNEL_H

#include "tinynet/base/Timestamp.h"
#include "tinynet/base/utils.h"

#include <functional>
#include <memory>

namespace tinynet
{
    namespace net
    {
        class EventLoop;

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

            void enableReading();
            void disableReading();
            void enableWriting();
            void disableWriting();
            void disableAll();

            bool isWriting() const { return _events & __WriteEvent; }
            bool isReading() const { return _events & __ReadEvent; }
            bool isNoneEvent() const { return _events == __NoneEvent; }

            void tie(const std::shared_ptr<void> &obj);
            void remove();

            int fd() const { return _fd; }
            int events() const { return _events; }
            int index() const { return _index; }
            void setREvents(int revent) { _revents = revent; }
            void setIndex(int idx) { _index = idx; }

            EventLoop *ownerLoop() { return _loop; }

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
            EventCallback _errorCb;
        };
    }
}

#endif // !TINYNET_NET_CHANNEL_H