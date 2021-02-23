/*
 * @Date: 2021-02-23 22:28:15
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-23 23:08:10
 * @FilePath: /tinynet/tinynet/net/EventLoopThread.h
 */

#ifndef TINYNET_NET_EVENTLOOPTHREAD_H
#define TINYNET_NET_EVENTLOOPTHREAD_H

#include "tinynet/base/Thread.h"
#include "tinynet/base/utils.h"

#include <condition_variable>
#include <functional>
#include <mutex>

namespace tinynet
{
    namespace net
    {
        class EventLoop;

        class EventLoopThread : noncopyable
        {
        public:
            using ThreadInitCallback = std::function<void(EventLoop *)>;

        public:
            EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                            const string &name = string());
            ~EventLoopThread();

            EventLoop *startLoop();

        private:
            void threadFunc();

        private:
            EventLoop *_loop;
            bool _exiting;
            Thread _thread;
            std::mutex _mtx;
            std::condition_variable _cv;
            ThreadInitCallback _callback;
        };

    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_EVENTLOOPTHREAD_H
