//
// Created by fxd on 23-4-13.
//

#include <sys/socket.h>
#include <unistd.h>

#include "Acceptor.h"
#include "logger/Logging.h"
#include "net/EventLoop.h"

namespace CppNet {
    namespace detail {
        int createSocket() {
            int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,IPPROTO_TCP);
            if (fd < 0) {
                LOG_FATAL << "createSocket::socket";
            }
            return fd;
        }
    }

    Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
        : loop_(loop),
          acceptSocket_(detail::createSocket()),
          acceptChannel_(loop, acceptSocket_.fd()),
          listening_(false) {
        acceptSocket_.setReuseAddr(true);
        acceptSocket_.bindAddress(listenAddr);
        acceptChannel_.setReadCallback([this](Timestamp t) {
            handleRead();
        });
    }

    Acceptor::~Acceptor() {
        acceptChannel_.disableAll();
        acceptChannel_.remove();
    }

    void Acceptor::listen() {
        loop_->assertInLoopThread();
        listening_ = true;
        acceptSocket_.listen();
        acceptChannel_.enableReading();
    }

    void Acceptor::handleRead() {
        loop_->assertInLoopThread();
        InetAddress peerAddr(0);
        int connfd = acceptSocket_.accept(&peerAddr);
        if (connfd > 0) {
            if (newConnectionCallback_) {
                newConnectionCallback_(connfd, peerAddr);
            } else {
                LOG_TRACE << "no newConnectionCallback";
                ::close(connfd);
            }
        } else {
            LOG_ERROR << "accept() error";
            if (errno == EMFILE) {
                LOG_ERROR << "sockfd reached limit";
            }
        }
    }


}