/*
 * @Date: 2021-02-24 10:02:09
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-27 14:52:50
 * @FilePath: /tinynet/tinynet/net/TcpConnection.h
 */

#ifndef TINYNET_NET_TCPCONNECTION_H
#define TINYNET_NET_TCPCONNECTION_H

#include "tinynet/base/utils.h"
#include "tinynet/net/Buffer.h"
#include "tinynet/net/Callbacks.h"
#include "tinynet/net/InetAddress.h"
#include "tinynet/net/Socket.h"

#include <memory>

namespace tinynet
{
    namespace net
    {
        class Socket;
        class EventLoop;
        class Channel;

        class TcpConnection : noncopyable,
                              public std::enable_shared_from_this<TcpConnection>
        {
            enum class StateE
            {
                __Disconnected,
                __Connecting,
                __Connected,
                __Disconnecting,
            };

        public:
            std::shared_ptr<TcpConnection> getptr()
            {
                return shared_from_this();
            }
            TcpConnection(EventLoop *loop,
                          const string &name,
                          int sockfd,
                          const InetAddress &localAddr,
                          const InetAddress &peerAddr);
            ~TcpConnection();

            void init();
            void send(const void *msg, size_t len);
            void send(const string &msg);
            void send(Buffer *bufferMsg); /// swap bufferMsg's data

            void shutdown();
            void forceClose();
            void forceCloseDelay(double seconds);
            void setTcpNoDelay(bool on = true);
            void startRead();
            void stopRead();

            void connectEstablished();
            void connectDestroyed();

            void setConnectionCallback(ConnectionCallback cb) { _connectionCallback = cb; }
            void setMessageCallback(MessageCallback cb) { _messageCallback = cb; }
            void setWriteCompleteCallback(WriteCompleteCallback cb) { _writeCompleteCallback = cb; }
            void setHighWaterMarkCallback(HighWaterMarkCallback cb, size_t highMark)
            {
                _highWaterMarkCallback = cb;
                _highWaterMark = highMark;
            }
            void setCloseCallback(CloseCallback cb) { _closeCallback = cb; }

            EventLoop *getLoop() const { return _loop; }
            const string &name() const { return _name; }
            const InetAddress &localAddress() const { return _localAddr; }
            const InetAddress &peerAddress() const { return _peerAddr; }
            Buffer *inputBuffer() { return &_inputBuffer; }
            Buffer *outputBuffer() { return &_outputBuffer; }
            bool connected() const { return _state == StateE::__Connected; }
            bool disConnected() const { return _state == StateE ::__Disconnected; }
            bool isReading() const { return _reading; }

        private:
            void handleRead(Timestamp receiveTime);
            void handleWrite();
            void handleClose();
            void handleError();
            void sendInLoop(const string &msg);
            void sendInLoop(const void *msg, size_t len);
            void shutdownInLoop();
            void forceCloseInLoop();
            void startReadInLoop();
            void stopReadInLoop();
            const char *stateToString() const;

            void setState(StateE state)
            {
                _state = state;
                printf("%s\n", stateToString());
            }

        private:
            EventLoop *_loop;
            string _name;
            StateE _state;
            bool _reading;
            std::unique_ptr<Socket> _socket;
            std::unique_ptr<Channel> _channel;
            const InetAddress _localAddr;
            const InetAddress _peerAddr;
            ConnectionCallback _connectionCallback;
            MessageCallback _messageCallback;
            WriteCompleteCallback _writeCompleteCallback;
            HighWaterMarkCallback _highWaterMarkCallback;
            CloseCallback _closeCallback;
            size_t _highWaterMark;
            Buffer _inputBuffer;
            Buffer _outputBuffer;
        };

    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_TCPCONNECTION_H
