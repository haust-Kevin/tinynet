/*
 * @Date: 2021-02-20 20:46:27
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-20 21:09:44
 * @FilePath: /tinynet/tinynet/net/Socket.h
 */

#ifndef TINYNET_NET_SOCKET_H
#define TINYNET_NET_SOCKET_H

#include "tinynet/base/utils.h"
#include "tinynet/net/InetAddress.h"

namespace tinynet
{
    namespace net
    {
        class Socket : noncopyable
        {
        public:
            explicit Socket(int sockfd) : _sockfd(sockfd)
            {
            }

            ~Socket();

            int fd() const { return _sockfd; }

            void bindAddress(const InetAddress &addr);
            void listen();
            int accept(InetAddress *clientAddr);
            void shutdownWrite();

            void setTcpNoDelay(bool on = true); /* SO_NODELAY */
            void setReuseAddr(bool on = true);  /* SO_REUSEADDR */
            void setReusePort(bool on = true);  /* SO_REUSEPORT */
            void setKeepAlive(bool on = true);  /* SO_KEEPALIVE */

        private:
            const int _sockfd;
        };
    }
}

#endif // !TINYNET_NET_SOCKET_H