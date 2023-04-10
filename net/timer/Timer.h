//
// Created by fxd on 23-4-10.
//

#ifndef CPPNET_TIMER_H
#define CPPNET_TIMER_H

#include <functional>
#include <utility>
#include "base/Timestamp.h"
#include "base/Noncopyable.h"

namespace CppNet  {
    class Timer : noncopyable{
    public:
        typedef std::function<void()> TimerCallback;
        Timer(TimerCallback cb, Timestamp when, double interval)
            : callback_(std::move(cb)),
              expiration_(when),
              interval_(interval),
              repeat_(interval > 0) {}
        void run() const {
            callback_();
        }

        Timestamp expiration() const { return expiration_; }
        bool repeat() const { return repeat_; }
        void restart(Timestamp now);

    private:
        TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        const bool repeat_;
    };

}

#endif //CPPNET_TIMER_H
