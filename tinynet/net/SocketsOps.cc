/*
 * @Date: 2021-02-19 17:50:36
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 16:50:01
 * @FilePath: /tinynet/tinynet/net/SocketsOps.cc
 */

#include "tinynet/net/SocketsOps.h"

#include "tinynet/net/endian.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h>
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
                     static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

void sockets::bind(int sockfd, const struct sockaddr_in *addr)
{
    int ret = ::bind(sockfd,
                     sockaddr_cast(addr),
                     static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    assert(sockfd >= 0);
}

void sockets::listen(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    assert(sockfd >= 0);
}

int sockets::accept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
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
size_t sockets::readv(int sockfd, const struct iovec *iov, int iocnt)
{
    return ::readv(sockfd, iov, iocnt);
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

void sockets::toIp(char *buff, size_t size, const struct sockaddr_in *addr)
{
    assert(size >= INET_ADDRSTRLEN);
    ::inet_ntop(AF_INET, &addr->sin_addr, buff, static_cast<socklen_t>(size));
}

void sockets::toIpPort(char *buff, size_t size, const struct sockaddr_in *addr)
{
    toIp(buff, size, addr);
    size_t end = strlen(buff);
    uint16_t port = sockets::networkToHost16(addr->sin_port);
    snprintf(buff + end, size - end, ":%u", port);
}

struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof localAddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localAddr);
    ::getsockname(sockfd, sockets::sockaddr_cast(&localAddr), &addrlen);
    return localAddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
    struct sockaddr_in peerAddr;
    memset(&peerAddr, 0, sizeof peerAddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peerAddr);
    ::getpeername(sockfd, sockets::sockaddr_cast(&peerAddr), &addrlen);
    return peerAddr;
}