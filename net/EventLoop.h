//
// Created by fxd on 23-4-9.
//

#ifndef CPPNET_EVENTLOOP_H
#define CPPNET_EVENTLOOP_H

#include <atomic>
#include <memory>

#include "base/Noncopyable.h"
#include "base/Timestamp.h"
#include "base/CurrentThread.h"
#include "net/Channel.h"
#include "net/Poller.h"

namespace CppNet {
    class EventLoop {
    public:
        EventLoop();
        ~EventLoop();

        void loop();
        void quit();

        void assertInLoopThread() {
            if (!isInLoopThread()) {
                abortNotInLoopThread();
            }
        }
        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);

    private:
        void abortNotInLoopThread();

        std::atomic_bool looping_;
        std::atomic_bool quit_;
        const pid_t threadId_;

        std::unique_ptr<Poller> poller_;
        Timestamp pollReturnTime_;
        std::vector<Channel*> activeChannels_;
    };

}

#endif //CPPNET_EVENTLOOP_H
