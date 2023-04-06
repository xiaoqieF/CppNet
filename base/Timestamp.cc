//
// Created by fxd on 23-4-5.
//

#include <cinttypes>
#include <sys/time.h>
#include "Timestamp.h"

namespace CppNet {
    std::string Timestamp::toString() const {
        char buf[32] = {0};
        auto seconds = static_cast<time_t> (microsecondsSinceEpoch_ / MicrosecondsPerSecond);
        int64_t microseconds = static_cast<int>(microsecondsSinceEpoch_ % MicrosecondsPerSecond);
        snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
        return buf;
    }

    std::string Timestamp::toFormatString() const {
        char buf[32] = {0};
        auto seconds = static_cast<time_t> (microsecondsSinceEpoch_ / MicrosecondsPerSecond);
        int microseconds = static_cast<int>(microsecondsSinceEpoch_ % MicrosecondsPerSecond);
        struct tm t;
        gmtime_r(&seconds, &t);

        snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d.%06d",
                 t.tm_year + 1900,
                 t.tm_mon + 1,
                 t.tm_mday,
                 t.tm_hour,
                 t.tm_min,
                 t.tm_sec,
                 microseconds);
        return buf;
    }

    Timestamp Timestamp::now() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return Timestamp(tv.tv_sec * MicrosecondsPerSecond + tv.tv_usec);
    }

    Timestamp Timestamp::invalid() {
        return Timestamp(0);
    }
}

