/*
 * @Date: 2021-02-19 08:50:17
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:56:13
 * @FilePath: /tinynet/tinynet/base/ThreadPool.h
 */

#ifndef TINYNET_BASE_THREADPOOL_H
#define TINYNET_BASE_THREADPOOL_H

#include "tinynet/base/Thread.h"
#include "tinynet/base/utils.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

namespace tinynet
{

    class ThreadPool : noncopyable
    {
    public:
        using Task = std::function<void()>;
        explicit ThreadPool(const string &name = string("threadpool"));
        ~ThreadPool();

        void start(int threadNum = 0);
        void stop();
        void run(Task task);
        size_t queueSize();

        void setMaxQueueSize(int maxSize) { _maxQueueSize = maxSize; }
        const string &name() const { return _name; }

    private:
        Task take();
        void runInThread();
        bool full();

    private:
        std::mutex _mtx;
        std::condition_variable _notFull;
        std::condition_variable _notEmpty;
        std::queue<Task> _taskQueue;
        std::vector<std::unique_ptr<Thread>> _threads;
        string _name;
        bool _running;
        size_t _maxQueueSize;
    };

} // namespace tinynet

#endif // !TINYNET_BASE_THREADPOOL_H