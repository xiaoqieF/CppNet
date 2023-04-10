//
// Created by fxd on 23-4-10.
//

#include "Timer.h"

namespace CppNet {

    void Timer::restart(Timestamp now) {
        if (repeat_) {
            expiration_ = addTime(now, interval_);
        } else {
            expiration_ = Timestamp::invalid();
        }
    }
}