//
// Created by fxd on 23-4-6.
//

#include "Logging.h"
#include "base/Timestamp.h"
#include "base/CurrentThread.h"
#include "logger/LogStream.h"

namespace CppNet {
    __thread char t_errnoBuf[512];
    __thread char t_time[32];
    __thread time_t t_lastSecond;

    const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
            "[TRACE] ",
            "[DEBUG] ",
            "[INFO ] ",
            "[WARN ] ",
            "[ERROR] ",
            "[FATAL] ",
    };
    const char* strerror_tl(int savedErrno) {
        return strerror_r(savedErrno, t_errnoBuf, sizeof(t_errnoBuf));
    }

    class Logger::Impl {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const char* file, int line)
            : time_(Timestamp::now()),
              stream_(),
              level_(level),
              line_(line),
              basename_(file)
        {
            formatTime();
            CurrentThread::tid();
            stream_.append(CurrentThread::tidString(), 6);
            stream_.append(LogLevelName[level], 8);
            if (old_errno != 0) {
                stream_ << strerror_tl(old_errno) << " (errno=" << old_errno << ") ";
            }
            stream_.append(basename_.data(), basename_.size());
            stream_ << ':' << line_ << " - ";
        }

        void formatTime();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    void Logger::Impl::formatTime() {
        int64_t microsecondsSinceEpoch = time_.microsecondsSinceEpoch();
        auto seconds = static_cast<time_t>(microsecondsSinceEpoch / Timestamp::MicrosecondsPerSecond);
        int microseconds = static_cast<int>(microsecondsSinceEpoch % Timestamp::MicrosecondsPerSecond);
        // 秒之前的部分 1 s 构造一次
        if (seconds != t_lastSecond) {
            t_lastSecond = seconds;
            struct tm t;
            ::gmtime_r(&seconds, &t);

            int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                               t.tm_year + 1900,
                               t.tm_mon + 1,
                               t.tm_mday,
                               t.tm_hour,
                               t.tm_min,
                               t.tm_sec);
            assert(len == 17);
        }
        Fmt us(".%06dZ ", microseconds);
        assert(us.length() == 9);
        stream_.append(t_time, 17);
        stream_.append(us.data(), 9);
    }

    Logger::LogLevel Logger::g_logLevel = INFO;
    Logger::OutputFunc Logger::g_output = [](const char* msg, int len) {
        fwrite(msg, 1, len, stdout);   // ok, thread safe
    };
    Logger::FlushFunc Logger::g_flush = []() {
        fflush(stdout);
    };

    Logger::Logger(const char* file, int line)
        : impl(new Impl(INFO, 0, file, line)) {}

    Logger::Logger(const char* file, int line, Logger::LogLevel level)
        : impl(new Impl(level, 0, file, line)) {}

    LogStream &Logger::stream() {
        return impl->stream_;
    }
    Logger::~Logger() {
        stream() << '\n';
        const LogStream::Buffer & buffer = impl->stream_.buffer();
        g_output(buffer.data(), buffer.length());
        if (impl->level_ == FATAL) {
            g_flush();
            abort();
        }
    }

}