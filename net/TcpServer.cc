//
// Created by fxd on 23-4-13.
//

#include "TcpServer.h"
#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/EventLoopThreadPool.h"
#include "net/TcpConnection.h"
#include "logger/Logging.h"

namespace CppNet {
    namespace detail {
        struct sockaddr_in getLocalAddr(int sockfd) {
            struct sockaddr_in localAddr;
            bzero(&localAddr, sizeof(localAddr));
            socklen_t len = sizeof(localAddr);
            if (::getsockname(sockfd, reinterpret_cast<sockaddr*>(&localAddr), &len)) {
                LOG_ERROR << "::getsockname";
            }
            return localAddr;
        }
    }
    void defaultConnectionCallback(const TcpConnectionPtr& conn) {
        LOG_TRACE << conn->localAddr().toIpPort() << " -> "
                  << conn->peerAddr().toIpPort() << " is "
                  << (conn->connected() ? "UP" : "DOWN");
    }
    void defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp) {
        buf->retrieveAll();
    }

    TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, std::string name)
        : loop_(checkLoopNotNull(loop)),
          hostPort_(listenAddr.toIpPort()),
          name_(std::move(name)),
          acceptor_(std::make_unique<Acceptor>(loop, listenAddr)),
          threadPool_(std::make_unique<EventLoopThreadPool>(loop)),
          connectionCallback_(defaultConnectionCallback),
          messageCallback_(defaultMessageCallback),
          started_(false),
          nextConnId_(1)
    {
        acceptor_->setNewConnectionCallback([this](int sockfd, const InetAddress& peerAddr) {
            newConnection(sockfd, peerAddr);
        });
    }

    TcpServer::~TcpServer() {
        loop_->assertInLoopThread();
        LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";
        for (auto& conn : connections_) {
            TcpConnectionPtr p = conn.second;
            conn.second.reset();
            p->getLoop()->runInLoop([p]() {
                p->connectDestroyed();
            });
        }
    }

    void TcpServer::setThreadNum(int numThreads) {
        threadPool_->setThreadNum(numThreads);
    }

    void TcpServer::start() {
        loop_->assertInLoopThread();
        if (!started_) {
            started_ = true;
            threadPool_->start(threadInitCallback_);
        }
        if (!acceptor_->listening()) {
            acceptor_->listen();
        }
    }

    void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
        loop_->assertInLoopThread();   // 在主线程
        EventLoop* ioLoop = threadPool_->getNextLoop();
        char buf[32];
        snprintf(buf, sizeof(buf), ":%s#%d", hostPort_.c_str(), nextConnId_);
        ++ nextConnId_;
        std::string connName = name_ + buf;
        LOG_INFO << "TcpServer::newConnection [" << name_
                 << "] - new connection [" << connName
                 << "] from " << peerAddr.toIpPort();
        InetAddress localAddr(detail::getLocalAddr(sockfd));
        TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop,
                                            connName, sockfd, localAddr, peerAddr);
        conn->setConnectionCallback(connectionCallback_);
        conn->setMessageCallback(messageCallback_);
        conn->setWriteCompleteCallback(writeCompleteCallback_);
        conn->setCloseCallback([this](const TcpConnectionPtr& p) {
            removeConnection(p);
        });
        ioLoop->runInLoop([conn]() {
            conn->connectEstablished();
        });
    }

    void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
        loop_->assertInLoopThread();
        LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
                 << "] - connection " << conn->name();
        size_t n = connections_.erase(conn->name());
        (void)n;
        assert(n == 1);
        EventLoop* ioLoop = conn->getLoop();
        ioLoop->queueInLoop([conn]() {
            conn->connectDestroyed();
        });
    }

}