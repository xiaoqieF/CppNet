//
// Created by fxd on 23-4-13.
//

#ifndef CPPNET_TCPCONNECTION_H
#define CPPNET_TCPCONNECTION_H

#include <memory>
#include <map>
#include <string>
#include <atomic>

#include "base/Noncopyable.h"
#include "net/InetAddress.h"
#include "net/Callbacks.h"
#include "net/Buffer.h"

namespace CppNet {
    class EventLoop;
    class Channel;
    class Socket;

    class TcpConnection : noncopyable,
            public std::enable_shared_from_this<TcpConnection>
    {
    public:
        TcpConnection(EventLoop* loop,
                      std::string name,
                      int sockfd,
                      const InetAddress& localAddr,
                      const InetAddress& peerAddr);
        ~TcpConnection();

        EventLoop* getLoop() const { return loop_; }
        const std::string& name() const { return name_; }
        const InetAddress& localAddr() const { return localAddr_; }
        const InetAddress& peerAddr() const { return peerAddr_; }
        bool connected() const { return state_ == kConnected; }

        void send(const void* message, size_t len);
        void send(const std::string& message);
        void send(Buffer* buf);

        void shutdown();
        void setTcpNoDelay(bool on);

        void setConnectionCallback(const ConnectionCallback& cb) {
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb) {
            messageCallback_ = cb;
        }
        void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
            writeCompleteCallback_ = cb;
        }
        void setCloseCallback(const CloseCallback& cb) {
            closeCallback_ = cb;
        }
        void setHighWaterMarkCallback(const HighWaterMarkCallback& cb) {
            highWaterMarkCallback_ = cb;
        }

        void connectEstablished();
        void connectDestroyed();

    private:
        enum StateE {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
        };

        void handleRead(Timestamp receiveTime);
        void handleWrite();
        void handleClose();
        void handleError();
        void sendInLoop(const void* message, size_t len);
        void shutdownInLoop();

        EventLoop* loop_;
        std::string name_;
        std::atomic<StateE> state_;
        std::unique_ptr<Channel> channel_;   // 持有 Channel
        std::unique_ptr<Socket> socket_;     // 持有 Socket

        InetAddress localAddr_;
        InetAddress peerAddr_;

        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        HighWaterMarkCallback highWaterMarkCallback_;
        CloseCallback closeCallback_;

        size_t highWaterMark_;
        Buffer inputBuffer_;
        Buffer outputBuffer_;
    };

}

#endif //CPPNET_TCPCONNECTION_H
