//
// Created by fxd on 23-4-5.
//

#ifndef CPPNET_TIMESTAMP_H
#define CPPNET_TIMESTAMP_H
#include <cstdint>
#include <string>

namespace CppNet {
    class Timestamp {
    public:
        Timestamp() : microsecondsSinceEpoch_(0) {}

        explicit Timestamp(int64_t microseconds) : microsecondsSinceEpoch_(microseconds) {}

        std::string toString() const;

        std::string toFormatString() const;

        bool isValid() const {
            return microsecondsSinceEpoch_ > 0;
        }

        int64_t microsecondsSinceEpoch() const {
            return microsecondsSinceEpoch_;
        }

        static Timestamp now();

        static Timestamp invalid();

        static const int MicrosecondsPerSecond = 1000 * 1000;

    private:
        int64_t microsecondsSinceEpoch_;
    };

    /// pass by value is ok
    inline bool operator<(Timestamp lhs, Timestamp rhs) {
        return lhs.microsecondsSinceEpoch() < rhs.microsecondsSinceEpoch();
    }

    inline bool operator==(Timestamp lhs, Timestamp rhs) {
        return lhs.microsecondsSinceEpoch() == rhs.microsecondsSinceEpoch();
    }

    inline Timestamp addTime(Timestamp timestamp, double seconds) {
        auto delta = static_cast<int64_t> (seconds * Timestamp::MicrosecondsPerSecond);
        return Timestamp(timestamp.microsecondsSinceEpoch() + delta);
    }

}
#endif //CPPNET_TIMESTAMP_H
