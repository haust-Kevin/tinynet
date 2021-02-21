/*
 * @Date: 2021-02-21 10:31:56
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-21 11:34:21
 * @FilePath: /tinynet/tinynet/base/Timestamp.h
 */

#ifndef TINYNET_BASE_TIMESTAMP_H
#define TINYNET_BASE_TIMESTAMP_H

#include "tinynet/base/utils.h"

namespace tinynet
{
    class Timestamp : copyable
    {
    public:
        Timestamp(int64_t milliseconds = 0);

        bool valid() const { return _mstp > 0; }
        int64_t milliSecondsSinceEpoch() { return _mstp; }
        void addTime(int milliseconds) { _mstp += milliseconds; }

    public:
        static Timestamp now();
        static Timestamp invalid();
        static Timestamp fromUnixTime(time_t t, int seconds);

    private:
        int64_t _mstp;
    };

    inline double timeDiff(Timestamp lhs, Timestamp rhs)
    {
        return (lhs.milliSecondsSinceEpoch() - rhs.milliSecondsSinceEpoch()) / 1000.0;
    }
}

#endif // !TINYNET_BASE_TIMESTAMP_H