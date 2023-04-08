//
// Created by fxd on 23-4-6.
//
#include <iostream>
#include <thread>
#include <memory>

#include "logger/Logging.h"
#include "base/CurrentThread.h"
#include "logger/FixedBuffer.h"

void threadFunc() {
    for (int i=0; i<10; ++i) {
        LOG_DEBUG << "threadID: " << CppNet::CurrentThread::tid();
    }
    LOG_WARN << CppNet::Fmt("%lf", 12.33312);
}

int main() {
    typedef CppNet::detail::FixedBuffer<CppNet::detail::kLargeBuffer> Buffer;
    std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>();
    auto buffer1 = std::move(buffer);
    if (buffer == nullptr) {
        LOG_INFO << "buffer is now nullptr";
    }

    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);

    std::thread t(threadFunc);

    LOG_TRACE << "hello";
    LOG_TRACE << "efefsef";
    LOG_INFO << "info";
    LOG_DEBUG << "debug: " << CppNet::Fmt("%04d", 23);
    LOG_ERROR << "error";

    t.join();
    return 0;
}