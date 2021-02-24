/*
 * @Date: 2021-02-24 10:06:14
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-24 12:50:51
 * @FilePath: /tinynet/tinynet/net/Buffer.cc
 */

#include "tinynet/net/Buffer.h"
#include "tinynet/net/SocketsOps.h"

#include <errno.h>
#include <sys/uio.h>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

size_t Buffer::readFd(int fd, int *savedErrno)
{
    char extraBuff[65536];
    struct iovec vec[2];
    size_t writeable = writableBytes();
    vec[0].iov_base = begin() + _writerIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extraBuff;
    vec[1].iov_len = sizeof extraBuff;
    int iovcnt = writeable < sizeof extraBuff ? 2 : 1;
    size_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (n <= writeable)
    {
        _writerIndex += n;
    }
    else
    {
        _writerIndex = _buffer.size();
        append(extraBuff, n - writeable);
    }
    return n;
}
