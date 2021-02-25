set(NET_SRCS
    SocketsOps.cc
    InetAddress.cc
    Socket.cc
    Poller.cc
    Channel.cc
    EventLoop.cc
    EventLoopThread.cc
    EventLoopThreadPool.cc
    Buffer.cc
    TcpConnection.cc
    Acceptor.cc
    poller/DefaultPoller.cc
)



add_library(tinynet_net ${NET_SRCS})

target_link_libraries(tinynet_net pthread)
