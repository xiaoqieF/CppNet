//
// Created by fxd on 23-4-9.
//

#include <thread>
#include <functional>
#include <unistd.h>

#include "net/EventLoop.h"
#include "logger/Logging.h"

CppNet::EventLoop* g_loop;

void handleRead(CppNet::Timestamp t, int fd) {
    char buf[64] = {0};
    ssize_t n = read(fd, buf, sizeof(buf));
    (void)n;
    g_loop->runEvery(1, [](){
        LOG_INFO << "call run after";
    });
}

int main() {
    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);
    CppNet::EventLoop loop;
    g_loop = &loop;

    CppNet::Channel channel(&loop, 0);
    channel.setReadCallback(std::bind(&handleRead, std::placeholders::_1, channel.fd()));
    channel.enableReading();
    loop.loop();
}