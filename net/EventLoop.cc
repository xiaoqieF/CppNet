//
// Created by fxd on 23-4-9.
//

#include <cassert>
#include <sys/eventfd.h>
#include <unistd.h>

#include "EventLoop.h"
#include "logger/Logging.h"
#include "net/poller/EpollPoller.h"

namespace CppNet {

    namespace detail {
        int createEventFd() {
            int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
            if (fd < 0) {
                LOG_FATAL << "::eventfd";
            }
            return fd;
        }
    }

    __thread EventLoop* t_loopInThisThread = nullptr;  // 线程局部变量，记录当前线程是否已经存在 loop
    const int kPollTimeout = 10000;  // ms

    EventLoop::EventLoop()
        : looping_(false),
          quit_(false),
          threadId_(CurrentThread::tid()),
          poller_(new EpollPoller(this)),
          timerQueue_(std::make_unique<TimerQueue>(this)),
          wakeupFd_(detail::createEventFd()),
          wakeupChannel_(new Channel(this, wakeupFd_)),
          callingPendingFunctors_(false)
    {
        LOG_TRACE << "EventLoop created " << this << " in thread: " << threadId_;
        if (t_loopInThisThread) {
            // one loop per thread
            LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                      << " exists in this thread: " << threadId_;
        } else {
            t_loopInThisThread = this;
        }
        wakeupChannel_->setReadCallback([this](Timestamp t) {
            handleWakeup();
        });
        wakeupChannel_->enableReading();
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
            doPendingFunctors();
        }

        looping_ = false;
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

    void EventLoop::runInLoop(const EventLoop::Functor &f) {
        if (isInLoopThread()) {
            f();
        } else {
            queueInLoop(f);
        }
    }

    void EventLoop::queueInLoop(const EventLoop::Functor &f) {
        {
            std::lock_guard<std::mutex> lockGuard(functorMutex_);
            pendingFunctors_.push_back(f);
        }
        if (!isInLoopThread() || callingPendingFunctors_) {
            wakeup();
        }
    }

    void EventLoop::handleWakeup() const {
        uint64_t one = 1;
        ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
    }

    void EventLoop::wakeup() const {
        uint64_t one = 1;
        ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
        if (n != sizeof(one)) {
            LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
        }
    }

    void EventLoop::doPendingFunctors() {
        std::vector<Functor> functors;
        callingPendingFunctors_ = true;

        {
            std::lock_guard<std::mutex> lockGuard(functorMutex_);
            functors.swap(pendingFunctors_);
        }

        for (auto& f : functors) {
            f();
        }
        callingPendingFunctors_ = false;
    }

    void EventLoop::runAt(Timestamp t, const EventLoop::Functor &f) {
        return timerQueue_->addTimer(f, t, 0.0);
    }

    void EventLoop::runAfter(double delay, const EventLoop::Functor &f) {
        Timestamp t(addTime(Timestamp::now(), delay));
        runAt(t, f);
    }

    void EventLoop::runEvery(double interval, const EventLoop::Functor &f) {
        Timestamp time(addTime(Timestamp::now(), interval));
        timerQueue_->addTimer(f, time, interval);
    }


}