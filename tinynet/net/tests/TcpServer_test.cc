/*
 * @Date: 2021-02-25 16:30:39
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-25 17:15:36
 * @FilePath: /tinynet/tinynet/net/tests/TcpServer_test.cc
 */

#include "tinynet/net/EventLoop.h"
#include "tinynet/net/InetAddress.h"
#include "tinynet/net/TcpServer.h"

using namespace std;
using namespace tinynet;
using namespace tinynet::net;

class EchoServer
{
    static const int __ThreadNum = 3;

public:
    EchoServer(EventLoop *loop, const InetAddress &listenAddr)
        : _loop(loop), _server(loop, listenAddr, "EchoServer")
    {
        _server.setConnectionCallback(bind(&EchoServer::onConnection, this, _1));
        _server.setMessageCallback(bind(&EchoServer::onMessage, this, _1, _2, _3));
        _server.setThreadNum(__ThreadNum);
    }

    void start()
    {
        _server.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        conn->send("hello\n");
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
    {
        string msg(buffer->extractAll());
        if (msg == "exit\n")
        {
            conn->send("bye\n");
            conn->shutdown();
        }
        if (msg == "quit\n")
        {
            // _loop->quit();
        }
        for (auto &c : msg)
        {
            c = toupper(c);
        }

        conn->send(msg);
    }

private:
    EventLoop *_loop;
    TcpServer _server;
};

int main(int argc, char const *argv[])
{
    EventLoop loop;
    InetAddress listenAddr(8880);
    EchoServer server(&loop, listenAddr);
    server.start();
    loop.loop();
    return 0;
}
