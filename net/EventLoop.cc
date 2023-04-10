//
// Created by fxd on 23-4-9.
//

#include <cassert>
#include "EventLoop.h"
#include "logger/Logging.h"
#include "net/poller/EpollPoller.h"

namespace CppNet {
    __thread EventLoop* t_loopInThisThread = nullptr;  // 线程局部变量，记录当前线程是否已经存在 loop
    const int kPollTimeout = 10000;  // ms

    EventLoop::EventLoop()
        : looping_(false),
          quit_(false),
          threadId_(CurrentThread::tid()),
          poller_(new EpollPoller(this))
    {
        LOG_TRACE << "EventLoop created " << this << " in thread: " << threadId_;
        if (t_loopInThisThread) {
            // one loop per thread
            LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                      << " exists in this thread: " << threadId_;
        } else {
            t_loopInThisThread = this;
        }
    }

    EventLoop::~EventLoop() {
        t_loopInThisThread = nullptr;
    }

    void EventLoop::abortNotInLoopThread() {
        LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
                  << " was created in threadId_ = " << threadId_
                  << ", current thread id = " <<  CurrentThread::tid();
    }

    void EventLoop::loop() {
        assertInLoopThread();
        looping_ = true;
        LOG_TRACE << "EventLoop " << this << " start looping";

        while (!quit_) {
            activeChannels_.clear();
            pollReturnTime_ = poller_->poll(kPollTimeout, &activeChannels_);
            for (auto c : activeChannels_) {
                c->handleEvent(pollReturnTime_);
            }
        }

        LOG_TRACE << "EventLoop " << this << " stop looping";
    }

    void EventLoop::quit() {
        quit_ = true;
    }

    void EventLoop::updateChannel(Channel *channel) {
        assert(channel->ownerLoop() == this);
        assertInLoopThread();
        poller_->updateChannel(channel);
    }

    void EventLoop::removeChannel(Channel *channel) {
        assert(channel->ownerLoop() == this);
        assertInLoopThread();
        poller_->removeChannel(channel);
    }

}