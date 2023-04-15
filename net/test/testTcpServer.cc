//
// Created by fxd on 23-4-14.
//

#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/TcpConnection.h"
#include "logger/Logging.h"

using namespace CppNet;

MessageCallback m = [](const TcpConnectionPtr& conn, Buffer* buf, Timestamp t) {
    LOG_DEBUG << "from " << conn->peerAddr().toIpPort() << ", message: " << buf->retrieveAllAsString();
    LOG_DEBUG << "Current thread: " << CurrentThread::tid();
};

int main() {
    Logger::setLogLevel(Logger::TRACE);
    EventLoop loop;
    TcpServer server(&loop, InetAddress(8888), "EchoServer");
    server.setThreadNum(4);
    server.setMessageCallback(m);
    server.start();
    loop.loop();
}