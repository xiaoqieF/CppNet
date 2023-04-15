//
// Created by fxd on 23-4-15.
//
//
// Created by fxd on 23-4-14.
//

#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/TcpConnection.h"
#include "logger/Logging.h"

using namespace CppNet;

class EchoServer : noncopyable {
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr)
            : loop_(loop),
              server_(loop, listenAddr, "EchoServer") {
        server_.setThreadNum(2);
        server_.setMessageCallback([](const TcpConnectionPtr& conn, Buffer* buf, Timestamp now) {
            onMessage(conn, buf, now);
        });
    }
    ~EchoServer() = default;
    void start() { server_.start(); }
private:
    static void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp now) {
        auto message = buf->retrieveAllAsString();
        LOG_DEBUG << "Got message: [" << message << "], Send back!";
        conn->send(message);
    }
    EventLoop* loop_;
    TcpServer server_;
};

int main() {
    Logger::setLogLevel(Logger::TRACE);
    EventLoop loop;

    EchoServer server(&loop, InetAddress(8888));
    server.start();

    loop.loop();
}
