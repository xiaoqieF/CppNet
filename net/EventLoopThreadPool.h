//
// Created by fxd on 23-4-11.
//

#ifndef CPPNET_EVENTLOOPTHREADPOOL_H
#define CPPNET_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>
#include <functional>
#include "base/Noncopyable.h"

namespace CppNet {
    class EventLoop;
    class EventLoopThread;

    class EventLoopThreadPool : noncopyable {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;
        explicit EventLoopThreadPool(EventLoop* baseLoop);
        ~EventLoopThreadPool();
        void setThreadNum(int numThreads) { numThreads_ = numThreads; }
        void start(const ThreadInitCallback& cb = ThreadInitCallback{});
        EventLoop* getNextLoop();

    private:
        EventLoop* baseLoop_;
        bool started_;
        int numThreads_;
        size_t next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*> loops_;
    };

}

#endif //CPPNET_EVENTLOOPTHREADPOOL_H
