/*
 * @Date: 2021-02-25 09:21:44
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 09:46:58
 * @FilePath: /tinynet/tinynet/net/Acceptor.h
 */
#ifndef TINYNET_NET_ACCEPTOR_H
#define TINYNET_NET_ACCEPTOR_H

#include "tinynet/net/Channel.h"
#include "tinynet/net/Socket.h"

namespace tinynet
{
    namespace net
    {
        class EventLoop;
        class InetAddress;

        class Acceptor : noncopyable
        {
            using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

        public:
            Acceptor(EventLoop *loop, const InetAddress &addr, bool reusePort = true);
            ~Acceptor();

            void listen();
            bool listening() const { return _listening; }
            void setNewConnectionCallback(const NewConnectionCallback &cb) { _newConnCallback = cb; }

        private:
            void handleRead();

        private:
            EventLoop *_loop;
            Socket _acceptSocket;
            Channel _acceptChannel;
            NewConnectionCallback _newConnCallback;
            bool _listening;
            int _idleFd;
        };

    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_ACCEPTOR_H