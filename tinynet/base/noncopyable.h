/*
 * @Date: 2021-02-18 21:50:16
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 16:55:53
 * @FilePath: /tinynet/tinynet/base/noncopyable.h
 */

#ifndef TINYNET_BASE_NONCOPYABLE_H
#define TINYNET_BASE_NONCOPYABLE_H

namespace tinynet
{

    class noncopyable
    {
    public:
        noncopyable(const noncopyable &) = delete;
        void operator=(const noncopyable &) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

} // namespace tinynet

#endif // !TINYNET_BASE_NONCOPYABLE_H
