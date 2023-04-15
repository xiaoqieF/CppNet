//
// Created by fxd on 23-4-15.
//
#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/TcpConnection.h"

using namespace CppNet;

class DaytimeServer {
public:
    DaytimeServer(EventLoop* loop, const InetAddress& listenAddr)
        : loop_(loop),
          server_(loop, listenAddr, "DaytimeServer") {
        server_.setConnectionCallback([](const TcpConnectionPtr& conn) {
            DaytimeServer::onConnection(conn);
        });
    }
    void start(int numThread=0) {
        server_.setThreadNum(numThread);
        server_.start();
    }

private:
    static void onConnection(const TcpConnectionPtr& conn) {
        conn->send(Timestamp::now().toFormatString());
        conn->shutdown();
    }
    EventLoop* loop_;
    TcpServer server_;
};

int main() {
    EventLoop loop;
    DaytimeServer server(&loop, InetAddress(9000));

    server.start();
    loop.loop();
}