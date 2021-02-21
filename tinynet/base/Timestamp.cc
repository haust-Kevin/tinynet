/*
 * @Date: 2021-02-21 10:31:45
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-21 11:21:30
 * @FilePath: /tinynet/tinynet/base/Timestamp.cc
 */
#include "tinynet/base/Timestamp.h"
#include "tinynet/base/utils.h"

#include <chrono>

using namespace std;
using namespace tinynet;
using namespace std::chrono;

Timestamp::Timestamp(int64_t milliseconds)
    : _mstp(milliseconds)
{
}

Timestamp Timestamp::now()
{
    auto tp = time_point_cast<milliseconds>(system_clock::now());
    auto tmp = duration_cast<milliseconds>(tp.time_since_epoch());
    return Timestamp(tmp.count());
}

inline Timestamp Timestamp::invalid()
{
    return Timestamp(0);
}

inline Timestamp Timestamp::fromUnixTime(time_t t, int seconds)
{
    return Timestamp(t * 1000 + seconds);
}