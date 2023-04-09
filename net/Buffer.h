//
// Created by fxd on 23-4-8.
//

#ifndef CPPNET_BUFFER_H
#define CPPNET_BUFFER_H

#include <vector>
#include <algorithm>
#include <cassert>
#include <string>

namespace CppNet {
    /// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
    ///
    /// @code
    /// +-------------------+------------------+------------------+
    /// | prependable bytes |  readable bytes  |  writable bytes  |
    /// |                   |     (CONTENT)    |                  |
    /// +-------------------+------------------+------------------+
    /// |                   |                  |                  |
    /// 0      <=      readerIndex   <=   writerIndex    <=     size
    /// @endcode
    class Buffer {
    public:
        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize = 1024;
        explicit Buffer(size_t initialSize = kInitialSize)
                :   buffer_(kCheapPrepend + initialSize),
                    readerIndex_(kCheapPrepend),
                    writerIndex_(kCheapPrepend) {}

        size_t readableBytes() const { return writerIndex_ - readerIndex_; }
        size_t writableBytes() const { return buffer_.size() - writerIndex_; }
        size_t prependBytes() const { return readerIndex_; }

        const char* peek() const { return begin() + readerIndex_; }
        const char* findCRLF() const {
            const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
            return crlf == beginWrite() ? nullptr : crlf;
        }
        void retrieve(size_t len) {
            if (len < readableBytes()) {
                readerIndex_ += len;
            } else {
                retrieveAll();
            }
        }
        void retrieveUntil(const char* end) {
            assert(peek() <= end);
            assert(end <= beginWrite());
            retrieve(end - peek());
        }
        void retrieveAll() {
            readerIndex_ =  kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }
        std::string retrieveAsString(size_t len) {
            assert(len <= readableBytes());
            std::string result(peek(), len);
            retrieve(len);
            return result;
        }
        std::string retrieveAllAsString() {
            return retrieveAsString(readableBytes());
        }
        void append(const char* data, size_t len) {
            ensureWritableBytes(len);
            std::copy(data, data+len, beginWrite());
            writerIndex_ += len;
        }
        void append(const std::string& str) {
            append(str.c_str(), str.size());
        }
        void ensureWritableBytes(size_t len) {
            if (writableBytes() < len) {
                makeSpace(len);
            }
        }
        ssize_t readFd(int fd, int* savedErrno);

        const char* beginWrite() const { return begin() + writerIndex_; }
        char* beginWrite() { return begin() + writerIndex_; }


    private:
        char* begin() { return &*buffer_.begin(); }
        const char* begin() const { return &*buffer_.begin(); }
        void makeSpace(size_t len) {
            if (len + readableBytes() + kCheapPrepend > buffer_.size()) {
                buffer_.resize(writerIndex_ + len);
            } else {
                size_t readable = readableBytes();
                std::copy(begin()+readerIndex_, begin()+writerIndex_, begin()+kCheapPrepend);
                readerIndex_ = kCheapPrepend;
                writerIndex_ = readerIndex_ + readable;
                assert(readable == readableBytes());
            }
        }

        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;

        static const char kCRLF[];
    };

}

#endif //CPPNET_BUFFER_H
