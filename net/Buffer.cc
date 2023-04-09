//
// Created by fxd on 23-4-8.
//

#include <sys/uio.h>

#include "Buffer.h"

namespace CppNet {
    const char Buffer::kCRLF[] = "\r\n";

    ssize_t Buffer::readFd(int fd, int *savedErrno) {
        char extraBuf[65536];
        struct iovec vec[2];
        size_t writable = writableBytes();
        vec[0].iov_base = beginWrite();
        vec[0].iov_len = writable;
        vec[1].iov_base = extraBuf;
        vec[1].iov_len = sizeof(extraBuf);
        ssize_t n = ::readv(fd, vec, 2);
        if (n < 0) {
            *savedErrno = errno;
        } else if (static_cast<size_t>(n) <= writable) {
            writerIndex_ += n;
        } else {
            writerIndex_ = buffer_.size();
            append(extraBuf, n-writable);
        }
        return n;
    }

    const size_t Buffer::kCheapPrepend;
    const size_t Buffer::kInitialSize;
}