/*
 * @Date: 2021-02-25 09:56:06
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 16:23:34
 * @FilePath: /tinynet/tinynet/net/TcpServer.cc
 */

#include "tinynet/net/TcpServer.h"
#include "tinynet/net/Acceptor.h"
#include "tinynet/net/EventLoop.h"
#include "tinynet/net/EventLoopThreadPool.h"

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

TcpServer::TcpServer(EventLoop *loop,
                     const InetAddress &addr,
                     const string &name,
                     Option option)
    : _loop(loop),
      _ipPort(addr.toIpPort()),
      _name(name),
      _acceptor(new Acceptor(loop, addr, option == Option::__ReusePort)),
      _threadPool(new EventLoopThreadPool(loop, name)),
      _connectionCallback(defaultConnectionCallback),
      _messageCallback(defaultMessageCallback),
      _nextConnId(1)
{
    _acceptor->setNewConnectionCallback(bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
    _loop->assertInLoopThread();
    for (auto &item : _connections)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int threadNum)
{
    assert(0 < threadNum);
    _threadPool->setThreadNum(threadNum);
}

void TcpServer::start()
{
    bool expected = false;
    if (_started.compare_exchange_strong(expected, true))
    {
        _threadPool->start(_threadInitCallback);
        assert(!_acceptor->listening());
        _loop->runInLoop(bind(&Acceptor::listen, _acceptor.get()));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    _loop->assertInLoopThread();
    EventLoop *loop = _threadPool->getNextLoop();
    char buff[64];
    snprintf(buff, sizeof buff, "-%s#%d", _ipPort.c_str(), _nextConnId);
    _nextConnId++;
    string connName = _name + buff;
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop, connName, sockfd, localAddr, peerAddr));
    _connections[connName] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);
    conn->setCloseCallback(bind(&TcpServer::removeConnection, this, _1));
    loop->runInLoop(bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    _loop->runInLoop(bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    _loop->assertInLoopThread();
    size_t n = _connections.erase(conn->name());
    assert(n == 1);
    EventLoop *loop = conn->getLoop();
    loop->queueInLoop(bind(&TcpConnection::connectDestroyed, conn));
}
