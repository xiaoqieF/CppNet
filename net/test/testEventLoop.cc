//
// Created by fxd on 23-4-9.
//

#include <thread>

#include "net/EventLoop.h"
#include "logger/Logging.h"


int main() {
    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);
    CppNet::EventLoop loop;
    loop.loop();
}