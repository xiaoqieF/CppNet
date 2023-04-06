//
// Created by fxd on 23-4-6.
//

#ifndef CPPNET_FIXEDBUFFER_H
#define CPPNET_FIXEDBUFFER_H

#include <string>
#include <cstring>
#include "base/Noncopyable.h"

namespace CppNet {
    namespace detail {
        const int kSmallBuffer = 4000;
        const int kLargeBuffer = 4000 * 1000;

        template<int SIZE>
        class FixedBuffer : noncopyable {
        public:
            FixedBuffer() : cur_(data_) {}
            ~FixedBuffer() = default;

            void append(const char* buf, size_t len) {
                if (static_cast<size_t>(avail()) > len) {
                    memcpy(cur_, buf, len);
                    cur_ += len;
                }
            }

            const char* data() const {
                return data_;
            }
            int length() const {
                return cur_ - data_;
            }
            char* current() {
                return cur_;
            }
            int avail() const {
                return end() - cur_;
            }
            void add(size_t len) {
                cur_ += len;
            }
            void reset() {
                cur_ = data_;
            }
            void bzero() {
                ::bzero(data_, sizeof(data_));
            }
            std::string toString() const {
                return std::string(data_, length());
            }

        private:
            const char* end() const {
                return data_ + sizeof(data_);
            }
            char data_[SIZE];
            char* cur_;
        };
    }
}

#endif //CPPNET_FIXEDBUFFER_H
