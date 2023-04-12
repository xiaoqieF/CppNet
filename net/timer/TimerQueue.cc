//
// Created by fxd on 23-4-10.
//

#include <unistd.h>
#include <sys/timerfd.h>

#include "TimerQueue.h"
#include "logger/Logging.h"
#include "net/EventLoop.h"

namespace CppNet {
    namespace detail {
        int createTimerFd() {
            int timerFd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
            if (timerFd < 0) {
                LOG_FATAL << "CppNet::detail::timerfd_create";
            }
            return timerFd;
        }

        struct timespec timeFromNow(Timestamp when) {
            int64_t microseconds = when.microsecondsSinceEpoch() - Timestamp::now().microsecondsSinceEpoch();
            if (microseconds < 100) {
                microseconds = 100;
            }
            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(
                    microseconds / Timestamp::MicrosecondsPerSecond);
            ts.tv_nsec = static_cast<long>(
                    (microseconds % Timestamp::MicrosecondsPerSecond) * 1000);
            return ts;
        }

        void resetTimerFd(int timerFd, Timestamp expiration) {
            struct itimerspec newValue;
            struct itimerspec oldValue;
            bzero(&newValue, sizeof(newValue));
            bzero(&oldValue, sizeof(oldValue));
            newValue.it_value = timeFromNow(expiration);
            int ret = ::timerfd_settime(timerFd, 0, &newValue, &oldValue);
            if (ret) {
                LOG_FATAL << "timerfd_settime()";
            }
        }

        void readTimerFd(int timerFd, Timestamp now) {
            uint64_t times;
            ssize_t n = ::read(timerFd, &times, sizeof(times));
            LOG_TRACE << "readTimerFd " << times;
            if (n != sizeof(times)) {
                LOG_ERROR << "readTimerFd, read " << n << " bytes instead of 8";
            }
        }
    }
    using namespace detail;

    TimerQueue::TimerQueue(EventLoop *loop)
        : loop_(loop),
          timerFd_(createTimerFd()),
          timerFdChannel_(loop, timerFd_),
          timers_()
    {
        timerFdChannel_.setReadCallback([this](Timestamp t) {
            handleRead();
        });
        timerFdChannel_.enableReading();
    }

    TimerQueue::~TimerQueue() {
        ::close(timerFd_);
    }

    void TimerQueue::handleRead() {
        loop_->assertInLoopThread();
        Timestamp now = Timestamp::now();
        readTimerFd(timerFd_, now);
        std::vector<Entry> expired = getExpired(now);

        for (const auto& entry : expired) {
            entry.second->run();
        }

        reset(expired, now);
    }

    std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
        // 获取当前超时的定时器，由于 timers_ 本身就是有序的，调用 lower_bound 即可
        std::vector<Entry> expired;
        auto it = timers_.lower_bound(Entry(now, nullptr));
        std::copy(timers_.begin(), it, std::back_inserter(expired));
        timers_.erase(timers_.begin(), it);
        return expired;
    }

    void TimerQueue::addTimer(const TimerQueue::TimerCallback &cb, Timestamp when, double interval) {
        auto timer = std::make_shared<Timer>(cb, when, interval);
        loop_->runInLoop([this, timer] (){
            addTimerInLoop(timer);
        });
    }

    void TimerQueue::addTimerInLoop(const std::shared_ptr<Timer>& t) {
        loop_->assertInLoopThread();
        bool earliestChanged = insert(t);
        if (earliestChanged) {
            resetTimerFd(timerFd_, t->expiration());
        }
    }

    bool TimerQueue::insert(const std::shared_ptr<Timer>& t) {
        loop_->assertInLoopThread();
        bool earliestChanged = false;
        Timestamp when = t->expiration();
        const auto& it = timers_.begin();
        if (it == timers_.end() || when < it->first) {
            earliestChanged = true;
        }
        auto res = timers_.insert(Entry(when, t));
        assert(res.second == true);

        return earliestChanged;
    }

    void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) {
        for (const auto& entry : expired) {
            if (entry.second->repeat()) {
                entry.second->restart(now);
                insert(entry.second);
            }
        }
        if (!timers_.empty()) {
            Timestamp nexExpire = timers_.begin()->second->expiration();
            resetTimerFd(timerFd_, nexExpire);
        }
    }

}