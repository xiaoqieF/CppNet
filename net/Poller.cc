//
// Created by fxd on 23-4-9.
//

#include "Poller.h"
#include "net/EventLoop.h"

namespace CppNet {
    Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {}

    void Poller::assertInLoopThread() {
        ownerLoop_->assertInLoopThread();
    }

    Poller::~Poller() = default;
}