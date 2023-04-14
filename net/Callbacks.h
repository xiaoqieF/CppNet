//
// Created by fxd on 23-4-13.
//

#ifndef CPPNET_CALLBACKS_H
#define CPPNET_CALLBACKS_H

#include <functional>
#include <memory>

namespace CppNet {
    class TcpConnection;
    class Buffer;
    class Timestamp;

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
    typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;
}

#endif //CPPNET_CALLBACKS_H
