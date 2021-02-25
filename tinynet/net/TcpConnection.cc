/*
 * @Date: 2021-02-24 10:01:57
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 09:10:52
 * @FilePath: /tinynet/tinynet/net/TcpConnection.cc
 */

#include "tinynet/net/TcpConnection.h"
#include "tinynet/base/Timestamp.h"
#include "tinynet/net/Channel.h"
#include "tinynet/net/EventLoop.h"
#include "tinynet/net/EventLoopThread.h"
#include "tinynet/net/EventLoopThreadPool.h"

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

TcpConnection::TcpConnection(EventLoop *loop,
                             const string &name,
                             int sockfd,
                             const InetAddress &localAddr,
                             const InetAddress &peerAddr)
    : _loop(loop),
      _name(name),
      _state(StateE::__Connecting),
      _reading(true),
      _socket(new Socket(sockfd)),
      _channel(new Channel(loop, sockfd)),
      _localAddr(localAddr),
      _peerAddr(peerAddr),
      _highWaterMark(64 * 1024 * 1024)
{
    _channel->setReadCallback(bind(&TcpConnection::handleRead, this, _1));
    _channel->setWriteCallback(bind(&TcpConnection::handleWrite, this));
    _channel->setCloseCallback(bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallback(bind(&TcpConnection::handleError, this));
    _socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    assert(_state == StateE::__Disconnected);
}

void TcpConnection::send(const void *msg, size_t len)
{
    if (_state == StateE::__Connected)
    {
        if (_loop->isInLoopThread())
        {
            sendInLoop(msg, len);
        }
        else
        {
            void (TcpConnection::*fp)(const void *msg, size_t len) = &TcpConnection::sendInLoop;
            _loop->runInLoop(bind(fp, this, msg, len));
        }
    }
}

void TcpConnection::send(const string &msg)
{
    send(msg.c_str(), msg.size());
}

void TcpConnection::send(Buffer *bufferMsg)
{
    if (_state == StateE::__Connected)
    {
        if (_loop->isInLoopThread())
        {
            sendInLoop(bufferMsg->peek(), bufferMsg->readableBytes());
            bufferMsg->retrieveAll();
        }
        else
        {
            void (TcpConnection::*fp)(const string &msg) = &TcpConnection::sendInLoop;
            _loop->runInLoop(bind(fp, this, bufferMsg->extractAll()));
        }
    }
}

void TcpConnection::shutdown()
{
    if (_state == StateE::__Connected)
    {
        setState(StateE::__Disconnecting);
        _loop->runInLoop(bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::forceClose()
{
    if (_state == StateE::__Connected || _state == StateE::__Disconnecting)
    {
        setState(StateE::__Connecting);
        _loop->queueInLoop(bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseDelay(double seconds)
{
    if (_state == StateE::__Connected || _state == StateE::__Disconnecting)
    {
        setState(StateE::__Connecting);
        // TODO 定时器任务
    }
}

void TcpConnection::setTcpNoDelay(bool on)
{
    _socket->setTcpNoDelay(on);
}

void TcpConnection::startRead()
{
    _loop->runInLoop(bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::stopRead()
{
    _loop->runInLoop(bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::connectEstablished()
{
    _loop->assertInLoopThread();
    assert(_state == StateE::__Connecting);
    setState(StateE::__Connected);
    _channel->tie(shared_from_this());
    _channel->enableReading();
}

void TcpConnection::connectDestroyed()
{
    _loop->assertInLoopThread();
    if (_state == StateE::__Connected)
    {
        setState(StateE::__Disconnected);
        _channel->disableAll();
        _connectionCallback(shared_from_this());
    }
    _channel->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    _loop->assertInLoopThread();
    int savedErrno = 0;
    size_t n = _inputBuffer.readFd(_channel->fd(), &savedErrno);
    if (n > 0)
    {
        _messageCallback(shared_from_this(), &_inputBuffer, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    _loop->assertInLoopThread();
    if (_channel->isWriting())
    {
        size_t n = sockets::write(_channel->fd(),
                                  _outputBuffer.peek(),
                                  _outputBuffer.readableBytes());
        if (n > 0)
        {
            _outputBuffer.retrieve(n);
            if (_outputBuffer.readableBytes() == 0)
            {
                _channel->disableWriting();
                if (_writeCompleteCallback)
                {
                    _loop->queueInLoop(bind(_writeCompleteCallback, shared_from_this()));
                }
                if (_state == StateE::__Disconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            // TODO log
        }
    }
}

void TcpConnection::handleClose()
{
    _loop->assertInLoopThread();
    assert(_state == StateE::__Connected || _state == StateE::__Disconnecting);
    setState(StateE::__Disconnected);
    _channel->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    _connectionCallback(guardThis);
    _closeCallback(guardThis);
}

void TcpConnection::handleError()
{
    // FIXME log
    assert(0);
}

void TcpConnection::sendInLoop(const string &msg)
{
    sendInLoop(msg.c_str(), msg.size());
}

void TcpConnection::sendInLoop(const void *msg, size_t len)
{
    _loop->assertInLoopThread();
    size_t nwrote = 0;
    size_t remaining = len;
    bool fault = false;
    if (_state == StateE::__Disconnected)
    {
        return;
    }
    if (!_channel->isWriting() && _outputBuffer.readableBytes() == 0)
    {
        nwrote = sockets::write(_channel->fd(), msg, len);
        if (nwrote >= 0)
        {
            remaining -= nwrote;
            if (remaining == 0 && _writeCompleteCallback)
            {
                _loop->queueInLoop(bind(_writeCompleteCallback, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    fault = true;
                }
            }
        }

        if (!fault && remaining > 0)
        {
            size_t oldLen = _outputBuffer.readableBytes();
            if (oldLen + remaining >= _highWaterMark &&
                oldLen < _highWaterMark &&
                _highWaterMarkCallback)
            {
                _loop->queueInLoop(
                    bind(_highWaterMarkCallback, shared_from_this(), oldLen + remaining));
            }
            _outputBuffer.append(static_cast<const char *>(msg) + nwrote, remaining);
            if (!_channel->isWriting())
            {
                _channel->enableWriting();
            }
        }
    }
}

void TcpConnection::shutdownInLoop()
{
    _loop->assertInLoopThread();
    if (!_channel->isWriting())
    {
        _socket->shutdownWrite();
    }
}

void TcpConnection::forceCloseInLoop()
{
    _loop->assertInLoopThread();
    if (_state == StateE::__Connected || _state == StateE::__Disconnecting)
    {
        handleClose();
    }
}

void TcpConnection::startReadInLoop()
{
    _loop->assertInLoopThread();
    if (!_reading || !_channel->isReading())
    {
        _channel->enableReading();
        _reading = true;
    }
}

void TcpConnection::stopReadInLoop()
{
    _loop->assertInLoopThread();
    if (_reading || _channel->isReading())
    {
        _channel->disableReading();
        _reading = false;
    }
}

const char *TcpConnection::stateToString() const
{
    switch (_state)
    {
    case StateE::__Disconnected:
        return "Disconnected";
    case StateE::__Disconnecting:
        return "Disconnecting";
    case StateE::__Connected:
        return "Connected";
    case StateE::__Connecting:
        return "Connecting";
    default:
        assert(0);
    }
    return "Unknown state";
}