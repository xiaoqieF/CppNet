//
// Created by fxd on 23-4-9.
//
#include <poll.h>

#include "Channel.h"
#include "logger/Logging.h"
#include "net/EventLoop.h"

namespace CppNet {
    const int Channel::kNoneEvent = 0;
    const int Channel::kReadEvent = POLLIN | POLLPRI;
    const int Channel::kWriteEvent = POLLOUT;

    Channel::Channel(EventLoop *loop, int fd)
        : loop_(loop),
          fd_(fd),
          events_(0),
          revents_(0),
          index_(-1) {}

    void Channel::update() {
        loop_->updateChannel(this);
    }

    void Channel::remove() {
        loop_->removeChannel(this);
    }

}