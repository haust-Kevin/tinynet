/*
 * @Date: 2021-02-18 22:11:13
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:43:17
 * @FilePath: /tinynet/tinynet/base/Thread.cc
 */

#include "tinynet/base/Thread.h"
#include "tinynet/base/utils.h"

#include <sstream>

using namespace std;
using namespace tinynet;

Thread::Thread(Thread::ThreadFunc func, const string &name)
    : _func(func), _started(false), _joined(false), _name(name)
{
    if (name == "")
    {
        setDefaultName();
    }
}

Thread::~Thread()
{
    if (_started && !_joined)
    {
        _thread.detach();
    }
}

void Thread::start()
{
    assert(!_started);
    _started = true;
    _thread = thread(_func);
}

void Thread::join()
{
    assert(_started && !_joined);
    _joined = true;
    _thread.join();
}

void Thread::setDefaultName()
{
    stringstream sos;
    sos << this_thread::get_id() << "-->" << _thread.get_id();
    _name = sos.str();
}