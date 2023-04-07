//
// Created by fxd on 23-4-7.
//

#include <cstdio>
#include <memory>
#include <string>
#include <unistd.h>

#include "LogFile.h"
#include "logger/Logging.h"


namespace CppNet {
    class LogFile::File : noncopyable {
    public:
        explicit File(const std::string& filename)
            : fp_(::fopen(filename.data(), "ae")),
              writtenBytes_(0) {
            ::setbuffer(fp_, buffer_, sizeof(buffer_));
        }
        ~File() {
            ::fclose(fp_);
        }

        void append(const char* logLine, size_t len);
        void flush() {
            ::fflush(fp_);
        }
        size_t writtenBytes() const { return writtenBytes_; }

    private:
        size_t write(const char* logLine, size_t len) {
            return ::fwrite_unlocked(logLine, 1, len, fp_);
        }
        FILE* fp_;
        char buffer_[64 * 1024];
        size_t writtenBytes_;
    };

    void LogFile::File::append(const char *logLine, const size_t len) {
        size_t written = 0;
        while (written != len) {
            size_t n = write(logLine + written, len - written);
            if (n == 0) {
                int err = ferror(fp_);
                if (err) {
                    fprintf(stderr, "LogFile::File::append() failed %s\n", strerror_tl(err));
                }
                break;
            }
            written += n;
        }
        writtenBytes_ += len;
    }

    LogFile::LogFile(std::string basename, size_t rollSize, int flushInterval)
        : basename_(std::move(basename)),
          rollSize_(rollSize),
          flushInterval_(flushInterval),
          count_(0),
          startOfPeriod_(0),
          lastRoll_(0),
          lastFlush_(0)
    {
        rollFile();
    }

    std::string LogFile::getLogFilename(const std::string &basename, time_t *now) {
        std::string filename;
        filename.reserve(basename.size() + 64);
        filename = basename;

        char timeBuf[32];
        char pidBuf[32];
        struct tm tm;
        *now = time(nullptr);
        localtime_r(now, &tm);
        strftime(timeBuf, sizeof(timeBuf), ".%Y%m%d-%H%M%S.", &tm);
        filename += timeBuf;
        snprintf(pidBuf, sizeof(pidBuf), "%d", ::getpid());
        filename += pidBuf;
        filename += ".log";

        return filename;
    }

    void LogFile::append(const char *logLine, int len) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        file_->append(logLine, len);

        if (file_->writtenBytes() > rollSize_) {
            rollFile();
        } else {
            ++count_;
            if (count_ >= kCheckTimeRoll_) {
                count_ = 0;
                time_t now = ::time(nullptr);
                time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
                if (thisPeriod_ != startOfPeriod_) {
                    rollFile();
                } else if (now - lastFlush_ > flushInterval_) {
                    lastFlush_ = now;
                    file_->flush();
                }
            }
        }
    }

    void LogFile::rollFile() {
        time_t now = 0;
        std::string filename = getLogFilename(basename_, &now);
        time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

        if (now > lastRoll_) {
            lastRoll_ = now;
            lastFlush_ = now;
            startOfPeriod_ = start;
            file_ = std::make_unique<File>(filename);
        }
    }

    void LogFile::flush() {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        file_->flush();
    }

    LogFile::~LogFile() = default;
}