/*
 * @Date: 2021-02-19 08:21:19
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:44:04
 * @FilePath: /tinynet/tinynet/base/tests/Thread_test.cc
 */

#include "tinynet/base/Thread.h"

#include <iostream>

using namespace std;
using namespace tinynet;
void task(int n)
{
    for (int i = 0; i < n; i++)
    {
        this_thread::sleep_for(chrono::seconds(1));
        cout << this_thread::get_id() << "睡眠第" << i + 1 << "次" << endl;
    }
}

int main(int argc, char const *argv[])
{
    /* code */
    Thread th1(bind(task, 3));
    Thread th2(bind(task, 3));
    cout << "main thread" << endl;

    th1.start();
    th2.start();
    th1.join();
    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}
