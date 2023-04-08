//
// Created by fxd on 23-4-8.
//

#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "Socket.h"
#include "logger/Logging.h"

namespace CppNet {

    Socket::~Socket() {
        ::close(sockfd_);
    }

    void Socket::bindAddress(const InetAddress &localAddr) {
        if (::bind(sockfd_, (sockaddr*)&localAddr.getSockAddrIn(), sizeof(sockaddr_in)) != 0) {
            LOG_FATAL << "Socket::bindAddress";
        }
    }

    void Socket::listen() {
        if (::listen(sockfd_, SOMAXCONN) != 0) {
            LOG_FATAL << "Socket::listen";
        }
    }

    int Socket::accept(InetAddress *peerAddr) {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        socklen_t len = sizeof(addr);
        // accept4 可以直接设置获得的 fd 的标志
        int connfd = ::accept4(sockfd_, (sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (connfd >= 0) {
            peerAddr->setSockAddrIn(addr);
        } else {
            LOG_FATAL << "Socket::accept";
        }
        return connfd;
    }

    void Socket::shutdownWrite() {
        if (::shutdown(sockfd_, SHUT_WR) < 0) {
            LOG_ERROR << "Socket::shutdownWrite";
        }
    }

    void Socket::setTcpNoDelay(bool on) {
        int val = on ? 1 : 0;
        ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
    }

    void Socket::setReuseAddr(bool on) {
        int val = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    }

    void Socket::setKeepAlive(bool on) {
        int val = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
    }
}