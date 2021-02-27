/*
 * @Date: 2021-02-25 19:00:32
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 20:00:16
 * @FilePath: /tinynet/tinynet/net/tests/EventLoop_test.cc
 */

#include <iostream>

#include "tinynet/net/EventLoop.h"
#include <chrono>
#include <functional>
#include <thread>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

void task(int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("***\n");
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main(int argc, char const *argv[])
{
    EventLoop loop;

    loop.runInLoop(bind(task, 4));
    cout << "-----" << endl;
    loop.runInLoop(bind(task, 4));
    cout << "-----" << endl;
    loop.queueInLoop(bind(task, 4));
    cout << "-----" << endl;
    loop.queueInLoop(bind(task, 4));
    cout << "-----" << endl;
    loop.queueInLoop(bind(task, 4));
    cout << "-----" << endl;
    loop.loop();
    this_thread::sleep_for(chrono::seconds(10));
    return 0;
}
