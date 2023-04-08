//
// Created by fxd on 23-4-7.
//

#ifndef CPPNET_ASYNCLOGGING_H
#define CPPNET_ASYNCLOGGING_H

#include <memory>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "base/Noncopyable.h"
#include "logger/FixedBuffer.h"

namespace CppNet {
    class AsyncLogging : noncopyable{
    public:
        AsyncLogging(std::string  base, size_t rollSize, int flushInterval=3);
        ~AsyncLogging() {
            if (running_) {
                stop();
            }
        }

        void append(const char* logLine, int len);  // 日志前端(生产者)调用该函数写入
        void start() {
            running_ = true;
            thread_ = std::make_unique<std::thread>(&AsyncLogging::theadFunc, this);
        }
        void stop() {
            if (running_) {
                running_ = false;
                cond_.notify_one();
                thread_->join();
            }
        }

    private:
        void theadFunc();    // 消费缓冲区中的日志

        typedef detail::FixedBuffer<detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int flushInterval_;
        bool running_;
        std::string basename_;
        size_t rollSize_;

        std::unique_ptr<std::thread> thread_;
        std::mutex mutex_;
        std::condition_variable cond_;

        BufferPtr currentBuffer_;
        BufferPtr nextBuffer_;
        BufferVector buffers_;
    };
}

#endif //CPPNET_ASYNCLOGGING_H
