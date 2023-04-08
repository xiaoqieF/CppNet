//
// Created by fxd on 23-4-7.
// LogFile向文件中写入是惰性的(块缓冲)，需要调用 flush() 才能将文件缓冲区内容写入文件
// LogFile本身的 flush 时机: 1. 写入文件内容超过 rollSize，关闭当前文件(冲洗缓冲区)，并新建一个文件
// 2. 调用 append 写入次数超过 kCheckTimeRoll_, 检查冲洗时间间隔，超过 3s 时冲洗缓冲区； 或者距离上次
// rollFile 时间超过 kRollPerSeconds_ 时 rollFile 一次(这时刷新缓冲区)。
//

#ifndef CPPNET_LOGFILE_H
#define CPPNET_LOGFILE_H

#include <string>
#include <memory>
#include <mutex>

#include "base/Noncopyable.h"

namespace CppNet {
    class LogFile : noncopyable{
    public:
        LogFile(std::string basename, size_t rollSize, int flushInterval=3);
        ~LogFile();

        void append(const char* logLine, int len);
        void flush();

    private:
        static std::string getLogFilename(const std::string& basename, time_t* now);
        void rollFile();

        const std::string basename_;
        const size_t rollSize_;
        const int flushInterval_;

        int count_;
        std::mutex mutex_;
        time_t startOfPeriod_;
        time_t lastRoll_;
        time_t lastFlush_;

        class File;
        std::unique_ptr<File> file_;

        const static int kCheckTimeRoll_ = 1024;   // 每写入 kCheckTimeRoll_ 次检查一次 flush
        const static int kRollPerSeconds_ = 60 * 60 * 24;
    };
}

#endif //CPPNET_LOGFILE_H
