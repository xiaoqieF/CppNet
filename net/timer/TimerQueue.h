//
// Created by fxd on 23-4-10.
//

#ifndef CPPNET_TIMERQUEUE_H
#define CPPNET_TIMERQUEUE_H

#include <set>
#include <vector>
#include <memory>

#include "base/Noncopyable.h"
#include "net/Channel.h"
#include "net/timer/Timer.h"

namespace CppNet {
    class EventLoop;

    class TimerQueue : noncopyable {
    public:
        typedef std::function<void()> TimerCallback;
        explicit TimerQueue(EventLoop* loop);
        ~TimerQueue();

        void addTimer(const TimerCallback& cb, Timestamp when, double interval);

    private:
        typedef std::pair<Timestamp, std::shared_ptr<Timer>> Entry;  // TODO: use unique_ptr
        typedef std::set<Entry> TimerList;

        void handleRead();
        void addTimerInLoop(const std::shared_ptr<Timer>& t);

        bool insert(const std::shared_ptr<Timer>& t);
        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry>& expired, Timestamp now);

        EventLoop* loop_;
        const int timerFd_;
        Channel timerFdChannel_;
        TimerList timers_;
    };

}

#endif //CPPNET_TIMERQUEUE_H
