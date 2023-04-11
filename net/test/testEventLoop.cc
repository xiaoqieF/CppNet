//
// Created by fxd on 23-4-9.
//

#include <thread>
#include <functional>
#include <unistd.h>
#include <vector>

#include "logger/Logging.h"
#include "net/EventLoopThread.h"


int main() {
    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);
    std::vector<std::unique_ptr<CppNet::EventLoopThread>> vec;
    for (int i=0; i < 4; ++i) {
        vec.emplace_back(new CppNet::EventLoopThread);
        vec[i]->startLoop();
    }
    std::this_thread::sleep_for(std::chrono::seconds(200));
}