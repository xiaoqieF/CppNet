//
// Created by fxd on 23-4-5.
//

#ifndef CPPNET_THREADPOOL_H
#define CPPNET_THREADPOOL_H

#include <functional>
#include <mutex>
#include <thread>
#include <memory>
#include <vector>
#include <deque>
#include <condition_variable>
#include "base/Noncopyable.h"

namespace CppNet {
    class ThreadPool : noncopyable{
    public:
        typedef std::function<void()> Task;
        explicit ThreadPool(std::string name = "");
        ~ThreadPool();

        void start(int numThreads);
        void stop();

        void run(const Task& f);

    private:
        void runInThread();

        std::mutex mutex_;
        std::condition_variable cv_;
        std::string name_;
        std::vector<std::unique_ptr<std::thread>> threads_;
        std::deque<Task> tasks_;
        bool running_;
    };
}

#endif //CPPNET_THREADPOOL_H