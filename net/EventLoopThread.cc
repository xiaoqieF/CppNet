//
// Created by fxd on 23-4-11.
//

#include <utility>
#include <cassert>
#include "EventLoopThread.h"
#include "net/EventLoop.h"


namespace CppNet {

    EventLoopThread::EventLoopThread(EventLoopThread::ThreadInitCallback cb)
        : loop_(nullptr),
          cb_(std::move(cb)) {}

    EventLoopThread::~EventLoopThread() {
        loop_->quit();
        if (thread_) {
            thread_->join();
        }
    }

    EventLoop *EventLoopThread::startLoop() {
        assert(thread_ == nullptr);
        thread_ = std::make_unique<std::thread>(&EventLoopThread::threadFunc, this);
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (loop_ == nullptr) {
                cond_.wait(lock);
            }
        }
        return loop_;
    }

    void EventLoopThread::threadFunc() {
        EventLoop loop;
        if (cb_) {
            cb_(&loop);
        }
        {
            std::lock_guard<std::mutex> lockGuard(mutex_);
            loop_ = &loop;
            cond_.notify_one();
        }
        loop.loop();
    }


}