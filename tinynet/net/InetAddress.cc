/*
 * @Date: 2021-02-20 19:50:04
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-20 21:11:57
 * @FilePath: /tinynet/tinynet/net/InetAddress.cc
 */

#include "tinynet/net/InetAddress.h"
#include "tinynet/net/endian.h"

using namespace tinynet;
using namespace tinynet;
using namespace tinynet::net;
using namespace tinynet::net::sockets;

InetAddress::InetAddress(uint16_t port)
{
    memset(&_addr, 0, sizeof _addr);
    _addr.sin_family = AF_INET;
    _addr.sin_port = sockets::hostToNetwork16(port);
    _addr.sin_addr.s_addr = sockets::hostToNetwork32(INADDR_ANY);
}

InetAddress::InetAddress(const string &ip, uint16_t port)
{
    memset(&_addr, 0, sizeof _addr);
    _addr.sin_family = AF_INET;
    _addr.sin_port = sockets::hostToNetwork16(port);
    assert(::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) > 0);
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
    : _addr(addr)
{
}

const struct sockaddr_in *InetAddress::getSockAddr() const
{
    return &_addr;
}