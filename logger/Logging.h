//
// Created by fxd on 23-4-6.
//

#ifndef CPPNET_LOGGING_H
#define CPPNET_LOGGING_H

#include <cstring>
#include <memory>

#include "logger/LogStream.h"

namespace CppNet {
    class SourceFile {
    public:
        explicit SourceFile(const char* filename) : data_(filename) {
            const char* slash = strrchr(filename, '/');
            if (slash) {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }
        const char* data() const { return data_; }
        int size() const { return size_; }

    private:
        const char* data_;
        int size_;
    };

    class Logger {
    public:
        enum LogLevel {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };

        Logger(const char* file, int line);
        Logger(const char* file, int line, LogLevel level);
        ~Logger();
        LogStream& stream();

    public:
        static LogLevel logLevel() { return g_logLevel; }
        static void setLogLevel(LogLevel level) { g_logLevel = level; }

        typedef void (*OutputFunc)(const char* msg, int len);
        typedef void (*FlushFunc)();
        static void setOutput(OutputFunc o) { g_output = o; }
        static void setFlush(FlushFunc f) { g_flush = f; }

    private:
        static LogLevel g_logLevel;
        static OutputFunc g_output;
        static FlushFunc g_flush;

        class Impl;
        std::unique_ptr<Impl> impl;
    };
    const char* strerror_tl(int savedErrno);

#define LOG_TRACE if(CppNet::Logger::logLevel() <= CppNet::Logger::TRACE) \
    CppNet::Logger(__FILE__, __LINE__, CppNet::Logger::TRACE).stream()
#define LOG_DEBUG if(CppNet::Logger::logLevel() <= CppNet::Logger::DEBUG) \
    CppNet::Logger(__FILE__, __LINE__, CppNet::Logger::DEBUG).stream()
#define LOG_INFO if(CppNet::Logger::logLevel() <= CppNet::Logger::INFO) \
    CppNet::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN CppNet::Logger(__FILE__, __LINE__, CppNet::Logger::WARN).stream()
#define LOG_ERROR CppNet::Logger(__FILE__, __LINE__, CppNet::Logger::ERROR).stream()
#define LOG_FATAL CppNet::Logger(__FILE__, __LINE__, CppNet::Logger::FATAL).stream()

}


#endif //CPPNET_LOGGING_H
