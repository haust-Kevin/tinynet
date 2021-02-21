/*
 * @Date: 2021-02-19 17:50:36
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 20:46:57
 * @FilePath: /tinynet/tinynet/net/SocketsOps.cc
 */

#include "tinynet/net/SocketsOps.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace tinynet;
using namespace tinynet::net;

void sockets::setNonBlockAndCloseOnExec(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
}

int sockets::createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    assert(sockfd >= 0);
    return sockfd;
}

int sockets::connect(int sockfd, const struct sockaddr_in *addr)
{
    return ::connect(sockfd, 
                    sockaddr_cast(addr), 
                    static_cast<socklen_t>(sizeof( struct sockaddr_in)));
}

void sockets::bind(int sockfd, const struct sockaddr_in *addr)
{
    int ret = ::bind(sockfd,
                     sockaddr_cast(addr),
                     static_cast<socklen_t>(sizeof( struct sockaddr_in)));
    assert(sockfd >= 0);
}

void sockets::listen(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    assert(sockfd >= 0);
}

int sockets::accept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof( struct sockaddr_in));
    int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
    setNonBlockAndCloseOnExec(connfd);
    assert(sockfd >= 0);
    return connfd;
}

size_t sockets::read(int sockfd, void *buff, size_t count)
{
    return ::read(sockfd, buff, count);
}

size_t sockets::write(int sockfd, const void *buff, size_t count)
{
    return ::write(sockfd, buff, count);
}

void sockets::close(int sockfd)
{
    int ret = ::close(sockfd);
    assert(ret >= 0);
}

void sockets::shutdownWrite(int sockfd)
{
    int ret = ::shutdown(sockfd, SHUT_WR);
    assert(ret >= 0);
}
