/*
 * @Date: 2021-02-23 23:03:00
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-23 23:24:28
 * @FilePath: /tinynet/tinynet/net/EventLoopThreadPool.h
 */

#ifndef TINYNET_NET_EVENTLOOPTHREADPOOL_H
#define TINYNET_NET_EVENTLOOPTHREADPOOL_H

#include "tinynet/base/utils.h"

#include <vector>

namespace tinynet
{
    namespace net
    {
        class EventLoop;
        class EventLoopThread;

        class EventLoopThreadPool : noncopyable
        {
        public:
            using ThreadInitCallback = std::function<void(EventLoop *)>;

        public:
            EventLoopThreadPool(EventLoop *loop, const string &name);
            ~EventLoopThreadPool();

            void start(const ThreadInitCallback &cb = ThreadInitCallback());
            EventLoop *getNextLoop();
            EventLoop *getLoopForHash(size_t hashCode);
            std::vector<EventLoop *> getAllLoops();

            void setThreadNum(int threadNum) { _threadNum = threadNum; }
            void started() const { return _started; }
            string &name() const { return _name; }

        private:
            EventLoop *_loop;
            string _name;
            bool _started;
            int _threadNum;
            int _next;
            std::vector<std::unique_ptr<EventLoopThread>> _threads;
            std::vector<EventLoop *> _loops;
        };

    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_EVENTLOOPTHREADPOOL_H