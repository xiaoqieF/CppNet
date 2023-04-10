//
// Created by fxd on 23-4-9.
//

#include <thread>
#include <functional>
#include <unistd.h>

#include "net/EventLoop.h"
#include "logger/Logging.h"

void handleRead(CppNet::Timestamp t, int fd) {
    char buf[64] = {0};
    ssize_t n = read(fd, buf, sizeof(buf));
    LOG_TRACE << "Read: " << n << " bytes, " << buf;
}

int main() {
    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);
    CppNet::EventLoop loop;

    CppNet::Channel channel(&loop, 0);
    channel.setReadCallback(std::bind(&handleRead, std::placeholders::_1, channel.fd()));
    channel.enableReading();
    channel.enableWriting();
    loop.loop();
}