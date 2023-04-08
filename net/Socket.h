//
// Created by fxd on 23-4-8.
//

#ifndef CPPNET_SOCKET_H
#define CPPNET_SOCKET_H

#include "base/Noncopyable.h"
#include "net/InetAddress.h"

namespace CppNet {
    class Socket : noncopyable{
    public:
        explicit Socket(int sockfd) : sockfd_(sockfd) {}
        ~Socket();

        int fd() const {
            return sockfd_;
        }
        void bindAddress(const InetAddress& localAddr);
        void listen();
        /// 获取非阻塞的(NONBLOCK | CLOEXEC)对方 socket
        /// 对方地址从参数 peerAddr 返回
        int accept(InetAddress* peerAddr);
        void shutdownWrite();

        /// 启用/禁用 Nagle 算法(TCP_NODELAY)
        void setTcpNoDelay(bool on);
        /// SO_REUSEADDR
        void setReuseAddr(bool on);
        /// SO_KEEPALIVE
        void setKeepAlive(bool on);

    private:
        const int sockfd_;
    };
}

#endif //CPPNET_SOCKET_H