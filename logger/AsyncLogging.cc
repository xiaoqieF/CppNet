//
// Created by fxd on 23-4-7.
//

#include <memory>
#include <utility>
#include <cassert>

#include "AsyncLogging.h"
#include "logger/LogFile.h"

namespace CppNet {

    AsyncLogging::AsyncLogging(std::string basename, size_t rollSize, int flushInterval)
        : flushInterval_(flushInterval),
          running_(false),
          basename_(std::move(basename)),
          rollSize_(rollSize),
          currentBuffer_(new Buffer),
          nextBuffer_(new Buffer),
          buffers_() {
        currentBuffer_->bzero();
        nextBuffer_->bzero();
        buffers_.reserve(16);
    }

    void AsyncLogging::append(const char *logLine, int len) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        // 当前缓冲区空间足够
        if (currentBuffer_->avail() > len) {
            currentBuffer_->append(logLine, len);
        } else {
            buffers_.emplace_back(currentBuffer_.release());
            if (nextBuffer_) {
                // 使用另一个缓冲区
                currentBuffer_ = std::move(nextBuffer_);   // now nextBuffer_ == nullptr
            } else {
                // 两个缓冲区都满了，重新分配一个缓冲区，很少发生
                currentBuffer_ = std::make_unique<Buffer>();
            }
            currentBuffer_->append(logLine, len);
            cond_.notify_one();
        }
    }

    void AsyncLogging::theadFunc() {
        assert(running_ == true);
        LogFile output(basename_, rollSize_);
        BufferPtr newBuffer1 = std::make_unique<Buffer>();
        BufferPtr newBuffer2 = std::make_unique<Buffer>();
        newBuffer1->bzero();
        newBuffer2->bzero();
        BufferVector buffersToWrite;
        buffersToWrite.reserve(16);

        while (running_) {
            assert(newBuffer1 && newBuffer1->length() == 0);
            assert(newBuffer2 && newBuffer2->length() == 0);
            assert(buffersToWrite.empty());

            {
                std::unique_lock<std::mutex> uniqueLock(mutex_);
                if (buffers_.empty()) {
                    // 如果后端 buffers 没有数据，等待 3 s
                    cond_.wait_for(uniqueLock, std::chrono::seconds(flushInterval_));
                }
                buffers_.emplace_back(currentBuffer_.release());
                currentBuffer_ = std::move(newBuffer1);
                buffersToWrite.swap(buffers_);
                if (!nextBuffer_) {
                    nextBuffer_ = std::move(newBuffer2);  // 此时 buffers_size() >= 2
                }
            }

            for (const auto& b : buffersToWrite) {
                output.append(b->data(), b->length());
            }
            if (buffersToWrite.size() > 2) {
                buffersToWrite.resize(2);
            }
            if (!newBuffer1) {
                newBuffer1 = std::move(buffersToWrite.back());
                buffersToWrite.pop_back();
                newBuffer1->reset();   // reset newBuffer1
            }
            if (!newBuffer2) {
                // 说明 newBuffer2 给了 nextBuffer_, buffersToWrite 必然还有 1 个 buffer
                newBuffer2 = std::move(buffersToWrite.back());
                buffersToWrite.pop_back();
                newBuffer2->reset();
            }
            buffersToWrite.clear();
            output.flush();
        }
        output.flush();
    }

}