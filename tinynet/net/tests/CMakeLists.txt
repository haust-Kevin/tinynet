



add_executable(tcpserver_test TcpServer_test.cc)
target_link_libraries(tcpserver_test tinynet_base tinynet_net)
add_test(NAME tcpserver_test COMMAND tcpserver_test)
