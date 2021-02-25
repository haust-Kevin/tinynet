/*
 * @Date: 2021-02-24 13:22:33
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 08:55:19
 * @FilePath: /tinynet/tinynet/net/Callbacks.h
 */

#ifndef TINYNET_NET_CALLBACKS_H
#define TINYNET_NET_CALLBACKS_H

#include "tinynet/base/Timestamp.h"

#include <functional>
#include <memory>

namespace tinynet
{
    using std::placeholders::_1;
    using std::placeholders::_2;

    namespace net
    {
        class TcpConnection;
        class Buffer;

        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
        using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
        using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
        using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;

        using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;

        void defaultConnectionCallback(const TcpConnectionPtr &conn);
        void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp receiveTime);
    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_CALLBACKS_H