/*
 * @Date: 2021-02-25 09:21:36
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 09:54:04
 * @FilePath: /tinynet/tinynet/net/Acceptor.cc
 */

#include "tinynet/net/Acceptor.h"
#include "tinynet/net/Channel.h"
#include "tinynet/net/EventLoop.h"
#include "tinynet/net/SocketsOps.h"

#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reusePort)
    : _loop(loop),
      _acceptSocket(sockets::createNonblocking()),
      _acceptChannel(loop, _acceptSocket.fd()),
      _listening(false),
      _idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(_idleFd >= 0);
    _acceptSocket.setReuseAddr();
    _acceptSocket.setReusePort(reusePort);
    _acceptSocket.bindAddress(addr);
    _acceptChannel.setReadCallback(bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    _acceptChannel.disableAll();
    _acceptChannel.remove();
    ::close(_idleFd);
}

void Acceptor::listen()
{
    _loop->assertInLoopThread();
    _listening = true;
    _acceptSocket.listen();
    _acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    _loop->assertInLoopThread();
    InetAddress peerAddr;
    int connfd = _acceptSocket.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (_newConnCallback)
        {
            _newConnCallback(connfd, peerAddr);
        }
        else
        {
            sockets::close(connfd);
        }
    }
    else
    {
        // log
        if (errno == EMFILE)
        {
            ::close(_idleFd);
            _idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}
