//
// Created by fxd on 23-4-13.
//

#ifndef CPPNET_TCPSERVER_H
#define CPPNET_TCPSERVER_H

#include <memory>
#include <atomic>
#include <unordered_map>

#include "base/Noncopyable.h"
#include "net/Acceptor.h"
#include "net/Callbacks.h"


namespace CppNet {
    class EventLoop;
    class Acceptor;
    class EventLoopThreadPool;

    class TcpServer : noncopyable {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;
        TcpServer(EventLoop* loop, const InetAddress& listenAddr, std::string name);
        ~TcpServer();

        const std::string& hostPort() const { return hostPort_; }
        const std::string& name() const { return name_; }

        void setThreadNum(int numThreads);
        void setThreadInitCallback(const ThreadInitCallback& cb) {
            threadInitCallback_ = cb;
        }
        void setConnectionCallback(const ConnectionCallback& cb) {
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb) {
            messageCallback_ = cb;
        }
        void setWriteCompleteCallback (const WriteCompleteCallback& cb) {
            writeCompleteCallback_ = cb;
        }

        // not thread safe, call it in loop thread
        void start();

    private:
        void newConnection(int sockfd, const InetAddress& peerAddr);
        void removeConnection(const TcpConnectionPtr& conn);

        EventLoop* loop_;
        const std::string hostPort_;
        const std::string name_;
        std::unique_ptr<Acceptor> acceptor_;
        std::unique_ptr<EventLoopThreadPool> threadPool_;

        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        ThreadInitCallback threadInitCallback_;

        std::atomic_bool started_;
        int nextConnId_;
        std::unordered_map<std::string, TcpConnectionPtr> connections_;
    };

}

#endif //CPPNET_TCPSERVER_H
