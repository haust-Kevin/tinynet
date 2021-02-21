/*
 * @Date: 2021-02-18 22:11:23
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:54:45
 * @FilePath: /tinynet/tinynet/base/Thread.h
 */

#ifndef TINYNET_BASE_THREAD_H
#define TINYNET_BASE_THREAD_H

#include "tinynet/base/utils.h"

#include <functional>
#include <thread>

namespace tinynet
{

    class Thread : noncopyable
    {
    public:
        using ThreadFunc = std::function<void()>;

        explicit Thread(ThreadFunc func, const string &name = string());
        ~Thread();

        void start();
        void join();

        const string &name() const { return _name; }
        bool started() const { return _started; }

    private:
        void setDefaultName();

    private:
        bool _started;
        bool _joined;
        std::thread _thread;
        ThreadFunc _func;
        string _name;
    };

} // namespace tinynet

#endif // !TINYNET_BASE_THREAD_H