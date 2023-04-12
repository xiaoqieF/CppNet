//
// Created by fxd on 23-4-9.
//
#include <poll.h>
#include <sstream>

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

    void Channel::handleEvent(Timestamp receiveTime) {
        if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
            LOG_WARN << "Channel::handleEvent POLLHUP";
            if (closeCallback_) {
                closeCallback_();
            }
        }
        if (revents_ & POLLNVAL) {
            LOG_WARN << "Channel::handleEvent POLLNVAL";
        }
        if (revents_ & (POLLERR | POLLNVAL)) {
            if (errorCallback_) {
                errorCallback_();
            }
        }
        if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
            if (readCallback_) {
                readCallback_(receiveTime);
            }
        }
        if (revents_ & POLLOUT) {
            if (writeCallback_) {
                writeCallback_();
            }
        }
    }

    std::string Channel::reventsToString() const {
        std::ostringstream oss;
        oss << fd_ << ": ";
        if (revents_ & POLLIN)
            oss << "IN ";
        if (revents_ & POLLPRI)
            oss << "PRI ";
        if (revents_ & POLLOUT)
            oss << "OUT ";
        if (revents_ & POLLHUP)
            oss << "HUP ";
        if (revents_ & POLLRDHUP)
            oss << "RDHUP ";
        if (revents_ & POLLERR)
            oss << "ERR ";
        if (revents_ & POLLNVAL)
            oss << "NVAL ";

        return oss.str();
    }

}