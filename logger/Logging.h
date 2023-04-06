//
// Created by fxd on 23-4-6.
//

#ifndef CPPNET_LOGGING_H
#define CPPNET_LOGGING_H

#include <cstring>

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

    private:

    };
}


#endif //CPPNET_LOGGING_H
