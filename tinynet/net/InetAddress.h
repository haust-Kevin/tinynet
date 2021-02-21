/*
 * @Date: 2021-02-20 19:50:34
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-21 09:50:09
 * @FilePath: /tinynet/tinynet/net/InetAddress.h
 */

#ifndef TINYNET_NET_INETADDRESS_H
#define TINYNET_NET_INETADDRESS_H

#include "tinynet/base/utils.h"
#include "tinynet/net/SocketsOps.h"

namespace tinynet
{
    namespace net
    {

        class InetAddress : copyable
        {
        public:
            explicit InetAddress(uint16_t port = 0);
            InetAddress(const string &ip, uint16_t port = 0);
            explicit InetAddress(const struct sockaddr_in &addr);

            const struct sockaddr_in *getSockAddr() const;

        private:
            struct sockaddr_in _addr;
        };
    }
}

#endif // !TINYNET_NET_INETADDRESS_H