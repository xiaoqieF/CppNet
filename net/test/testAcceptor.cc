//
// Created by fxd on 23-4-13.
//

#include <unistd.h>
#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "logger/Logging.h"

using namespace CppNet;

void onConnection(int sockfd, const InetAddress& addr) {
    LOG_INFO << "onConnection, sockfd: " << sockfd << ", addr: " << addr.toIpPort();
    write(sockfd, "hello\n", 6);
    close(sockfd);
}

int main() {
    Logger::setLogLevel(Logger::TRACE);
    CppNet::EventLoop loop;
    Acceptor acceptor(&loop, InetAddress(8888));
    acceptor.setNewConnectionCallback(onConnection);
    acceptor.listen();
    loop.loop();
}