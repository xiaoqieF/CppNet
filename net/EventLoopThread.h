//
// Created by fxd on 23-4-11.
//

#ifndef CPPNET_EVENTLOOPTHREAD_H
#define CPPNET_EVENTLOOPTHREAD_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

#include "base/Noncopyable.h"

namespace CppNet {
    class EventLoop;

    class EventLoopThread : noncopyable {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;
        explicit EventLoopThread(ThreadInitCallback  cb = ThreadInitCallback{});
        ~EventLoopThread();
        EventLoop* startLoop();

    private:
        void threadFunc();

        EventLoop* loop_;
        std::unique_ptr<std::thread> thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
        ThreadInitCallback cb_;
    };

}

#endif //CPPNET_EVENTLOOPTHREAD_H
