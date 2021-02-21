/*
 * @Date: 2021-02-20 20:46:19
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-20 22:32:11
 * @FilePath: /tinynet/tinynet/net/Socket.cc
 */

#include "tinynet/net/Socket.h"

#include <netinet/tcp.h>

using namespace tinynet;
using namespace tinynet::net;

Socket::~Socket()
{
    sockets::close(_sockfd);
}

void Socket::bindAddress(const InetAddress &addr)
{
    sockets::bind(_sockfd, addr.getSockAddr());
}

void Socket::listen()
{
    sockets::listen(_sockfd);
}

int Socket::accept(InetAddress *clientAddr)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    return sockets::accept(_sockfd,
                           const_cast<struct sockaddr_in *>(clientAddr->getSockAddr()));
}

void Socket::shutdownWrite()
{
    sockets::shutdownWrite(_sockfd);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY,
                 &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,
                 &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,
                 &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE,
                 &optval, static_cast<socklen_t>(sizeof optval));
}
