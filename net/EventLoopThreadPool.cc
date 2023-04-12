//
// Created by fxd on 23-4-11.
//
#include <cassert>

#include "EventLoopThreadPool.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"

namespace CppNet {

    EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
        : baseLoop_(baseLoop),
          started_(false),
          numThreads_(0),
          next_(0) {}

    EventLoopThreadPool::~EventLoopThreadPool() = default;

    void EventLoopThreadPool::start(const EventLoopThreadPool::ThreadInitCallback &cb) {
        assert(!started_);
        // 保证在 baseLoop 所在的线程执行，避免多个线程执行当前函数导致竞争
        baseLoop_->assertInLoopThread();

        started_ = true;
        for (int i=0; i<numThreads_; ++i) {
            threads_.emplace_back(new EventLoopThread(cb));
            loops_.emplace_back(threads_[i]->startLoop());
        }
        if (numThreads_ == 0 && cb) {
            cb(baseLoop_);
        }
    }

    EventLoop *EventLoopThreadPool::getNextLoop() {
        baseLoop_->assertInLoopThread();
        EventLoop* loop = baseLoop_;

        if (!loops_.empty()) {
            // round-robin
            loop = loops_[next_];
            next_ = (next_ + 1) % loops_.size();
        }
        return loop;
    }
}