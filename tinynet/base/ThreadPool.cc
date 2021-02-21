/*
 * @Date: 2021-02-19 08:49:49
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:42:05
 * @FilePath: /tinynet/tinynet/base/ThreadPool.cc
 */

#include "tinynet/base/ThreadPool.h"
#include "tinynet/base/utils.h"

using namespace std;
using namespace tinynet;

ThreadPool::ThreadPool(const string &name)
    : _name(name), _maxQueueSize(0), _running(false)
{
}

ThreadPool::~ThreadPool()
{
    if (_running)
    {
        stop();
    }
}

void ThreadPool::start(int threadNum)
{
    assert(_threads.empty());
    _running = true;
    _threads.reserve(threadNum);
    for (int i = 0; i < threadNum; i++)
    {
        _threads.emplace_back(
            make_unique<Thread>(bind(&ThreadPool::runInThread, this), _name + to_string(i + 1)));
        _threads[i]->start();
    }
}

void ThreadPool::stop()
{
    {
        lock_guard<mutex> guard(_mtx);
        _running = false;
        _notEmpty.notify_all();
        _notFull.notify_all();
    }
    for (auto &th : _threads)
    {
        th->join();
    }
    _threads.clear();
}

void ThreadPool::run(ThreadPool::Task task)
{
    assert(_running);
    if (_threads.empty())
    {
        task();
    }
    else
    {
        unique_lock<mutex> lock(_mtx);
        _notFull.wait(lock, [this]() { return !full() || !_running; });
        if (_running)
        {
            _taskQueue.push(task);
            _notEmpty.notify_one();
        }
    }
}

size_t ThreadPool::queueSize()
{
    lock_guard<mutex> guard(_mtx);
    return _taskQueue.size();
}

ThreadPool::Task ThreadPool::take()
{
    unique_lock<mutex> lock(_mtx);
    _notEmpty.wait(lock, [this]() { return !_taskQueue.empty() || !_running; });
    Task task;
    if (_running)
    {
        task = _taskQueue.front();
        _taskQueue.pop();
        _notFull.notify_one();
    }
    return task;
}

void ThreadPool::runInThread()
{
    while (_running)
    {
        Task task(take());
        if (task)
        {
            task();
        }
    }
}

bool ThreadPool::full()
{
    // assert(!_mtx.try_lock());
    return _maxQueueSize > 0 && _taskQueue.size() >= _maxQueueSize;
}