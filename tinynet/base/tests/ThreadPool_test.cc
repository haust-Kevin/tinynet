/*
 * @Date: 2021-02-19 12:24:42
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:41:34
 * @FilePath: /tinynet/tinynet/base/tests/ThreadPool_test.cc
 */

#include "tinynet/base/ThreadPool.h"

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
    ThreadPool pool("test_pool");
    pool.start(5);

    cout << "main thread" << endl;
    for (int i = 0; i < 10; i++)
    {
        pool.run(bind(task, 3));
    }
    this_thread::sleep_for(chrono::seconds(4));
    return 0;
}
