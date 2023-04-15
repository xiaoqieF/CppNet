//
// Created by fxd on 23-4-6.
//

#ifndef CPPNET_LOGSTREAM_H
#define CPPNET_LOGSTREAM_H

#include <cassert>
#include "base/Noncopyable.h"
#include "logger/FixedBuffer.h"

namespace CppNet {
    class LogStream : noncopyable{
        typedef LogStream self;
    public:
        typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

        self& operator<<(bool v) {
            buffer_.append(v ? "1" : "0", 1);
            return *this;
        }
        self& operator<<(short);
        self& operator<<(unsigned short);
        self& operator<<(int);
        self& operator<<(unsigned int);
        self& operator<<(long);
        self& operator<<(unsigned long);
        self& operator<<(long long);
        self& operator<<(unsigned long long);

        self& operator<<(float v);
        self& operator<<(double);
        self& operator<<(char v) {
            buffer_.append(&v, 1);
            return *this;
        }
        self& operator<<(const char* v) {
            buffer_.append(v, strlen(v));
            return *this;
        }
        self& operator<<(const void* v);
        self& operator<<(const std::string& v) {
            buffer_.append(v.c_str(), v.size());
            return *this;
        }

        void append(const char* data, size_t len) {
            buffer_.append(data, len);
        }
        const Buffer& buffer() const {
            return buffer_;
        }
        void resetBuffer() {
            buffer_.reset();
        }
    private:
        static const int kMaxNumericSize = 48;
        template<typename T>
        void formatInteger(T);  // 这里模板实现不在头文件可以，因为没有其他类外部的访问(private的模板函数可以在cpp实现)
        Buffer buffer_;
    };

    class Fmt {
    public:
        template<typename T>
        Fmt(const char* fmt, T val);

        const char* data() const {
            return buf_;
        }
        int length() const {
            return length_;
        }

    private:
        char buf_[32];
        int length_;
    };

    template<typename T>
    Fmt::Fmt(const char *fmt, T val) {
        length_ = snprintf(buf_, sizeof(buf_), fmt, val);
        assert(static_cast<size_t>(length_) < sizeof(buf_));
    }

    inline LogStream& operator<<(LogStream& s, const Fmt& fmt) {
        s.append(fmt.data(), fmt.length());
        return s;
    }
}

#endif //CPPNET_LOGSTREAM_H
