


add_executable(eventloop_test EventLoop_test.cc)
target_link_libraries(eventloop_test tinynet_base tinynet_net)
add_test(NAME eventloop_test COMMAND eventloop_test)

add_executable(tcpserver_test TcpServer_test.cc)
target_link_libraries(tcpserver_test tinynet_base tinynet_net)
add_test(NAME tcpserver_test COMMAND tcpserver_test)
