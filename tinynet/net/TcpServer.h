/*
 * @Date: 2021-02-25 09:55:56
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 17:06:51
 * @FilePath: /tinynet/tinynet/net/TcpServer.h
 */

#ifndef TINYNET_NET_TCPSERVER_H
#define TINYNET_NET_TCPSERVER_H

#include "tinynet/base/utils.h"
#include "tinynet/net/Callbacks.h"
#include "tinynet/net/TcpConnection.h"

#include <atomic>
#include <map>

namespace tinynet
{
    namespace net
    {
        class EventLoopThreadPool;
        class Acceptor;

        class TcpServer : noncopyable
        {
            enum class Option
            {
                __NoReusePort,
                __ReusePort
            };

            using ThreadInitCallback = std::function<void(EventLoop *)>;
            using ConnectionMap = std::map<string, TcpConnectionPtr>;

        public:
            TcpServer(EventLoop *loop,
                      const InetAddress &addr,
                      const string &name,
                      Option option = Option::__NoReusePort);
            ~TcpServer();

            void setThreadNum(int threadNum);
            void start();

            void setThreadInitCallback(const ThreadInitCallback &cb) { _threadInitCallback = cb; }
            void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback = cb; }
            void setMessageCallback(const MessageCallback &cb) { _messageCallback = cb; }
            void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback = cb; }

            std::shared_ptr<EventLoopThreadPool> threadPool() { return _threadPool; }
            const string &ipPort() const { return _ipPort; }
            const string &name() const { return _name; }
            EventLoop *getLoop() const { return _loop; }

        private:
            void newConnection(int sockfd, const InetAddress &peerAddr);
            void removeConnection(const TcpConnectionPtr &conn);
            void removeConnectionInLoop(const TcpConnectionPtr &conn);

        private:
            EventLoop *_loop;
            const string _ipPort;
            const string _name;
            std::unique_ptr<Acceptor> _acceptor;
            std::shared_ptr<EventLoopThreadPool> _threadPool;
            ConnectionCallback _connectionCallback;
            MessageCallback _messageCallback;
            WriteCompleteCallback _writeCompleteCallback;
            ThreadInitCallback _threadInitCallback;
            std::atomic<bool> _started;
            int _nextConnId;
            ConnectionMap _connections;
        };

    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_TCPSERVER_H