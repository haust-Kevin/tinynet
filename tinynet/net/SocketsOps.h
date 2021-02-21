/*
 * @Date: 2021-02-19 17:50:47
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-20 20:33:40
 * @FilePath: /tinynet/tinynet/net/SocketsOps.h
 */

#ifndef TINYNET_NET_SOCKETSOPS_H
#define TINYNET_NET_SOCKETSOPS_H

#include "tinynet/base/utils.h"

#include <arpa/inet.h>

namespace tinynet
{
    namespace net
    {
        namespace sockets
        {

            int createNonblocking();
            void setNonBlockAndCloseOnExec(int sockfd);
            int connect(int sockfd, const struct sockaddr_in *addr);
            void bind(int sockfd, const struct sockaddr_in *addr);
            int accept(int sockfd, struct sockaddr_in *addr);
            void listen(int sockfd);
            void close(int sockfd);
            void shutdownWrite(int sockfd);
            size_t read(int sockfd, void *buff, size_t count);
            size_t write(int sockfd, const void *buff, size_t count);

            const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr);
            struct sockaddr *sockaddr_cast(struct sockaddr_in *addr);

            inline const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr)
            {
                return static_cast<const struct sockaddr *>(static_cast<const void *>(addr));
            }

            inline struct sockaddr *sockaddr_cast(struct sockaddr_in *addr)
            {
                return static_cast<struct sockaddr *>(static_cast<void *>(addr));
            }

        } // namespace sockets

    } // namespace net
} // namespace tinynet

#endif // !TINYNET_NET_SOCKETSOPS_H