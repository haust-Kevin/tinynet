/*
 * @Date: 2021-02-21 10:10:00
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-22 20:23:14
 * @FilePath: /tinynet/tinynet/net/EventLoop.h
 */

#ifndef TINYNET_NET_EVENTLOOP_H
#define TINYNET_NET_EVENTLOOP_H

#include "tinynet/base/utils.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace tinynet
{
    namespace net
    {
        class Channel;
        class Poller;

        class EventLoop : noncopyable
        {
            using ChannelList = std::vector<Channel *>;
            using Functor = std::function<void()>;

        public:
            EventLoop();
            ~EventLoop();

            void loop();
            void quit();
            Timestamp pollReturnTime() const { return _pollReturnTime; }

            void wakeup();
            void updateChannel(Channel *channel);
            void removeChannel(Channel *channel);
            bool hasChannel(Channel *channel);

            void runInLoop(Functor func);
            void queueInLoop(Functor func);
            size_t queueSize() const;
            /*
             * *****************
             * // TODO 定时器任务
             * *****************
             */
            bool isInLoopThread() const;
            void assertInLoopThread() const;
            bool eventHandling() const { return _eventHandling; }

            static EventLoop *getEventLoopOfCurrentThread();

        private:
            void handleRead();
            void doPendingFunctorsd();

        private:
            bool _looping;
            std::atomic<bool> _quit;
            bool _eventHandling;
            bool _callingPendingFunctors;
            int64_t _iteration;
            const std::thread::id _tid;
            Timestamp _pollReturnTime;
            std::unique_ptr<Poller> _poller;
            // std::unique_ptr<TimeQueue> _timeQueue;
            int _wakeupFd;
            std::unique_ptr<Channel> _wakeupChannel;
            ChannelList _activeChannels;
            Channel *_currentActiveChannel;
            mutable std::mutex _mtx;
            std::vector<Functor> pendingFunctors;
        };
    }
}

#endif // !TINYNET_NET_EVENTLOOP_H