//
// Created by fxd on 23-4-13.
//

#ifndef CPPNET_ACCEPTOR_H
#define CPPNET_ACCEPTOR_H

#include <functional>

#include "base/Noncopyable.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "Channel.h"

namespace CppNet {
    class EventLoop;
    class Acceptor : noncopyable {
    public:
        typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
        Acceptor(EventLoop* loop, const InetAddress& listenAddr);
        ~Acceptor();

        void setNewConnectionCallback(const NewConnectionCallback& cb) {
            newConnectionCallback_ = cb;
        }
        bool listening() const { return listening_; }
        void listen();

    private:
        void handleRead();   // 新连接到来后回调

        EventLoop* loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listening_;
    };

}

#endif //CPPNET_ACCEPTOR_H
