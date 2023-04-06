//
// Created by fxd on 23-4-5.
//

#include <cassert>
#include <utility>

#include "ThreadPool.h"

namespace CppNet {

    ThreadPool::ThreadPool(std::string name) : name_(std::move(name)), running_(false) {}

    ThreadPool::~ThreadPool() {
        if (running_) {
            stop();
        }
    }

    void ThreadPool::start(int numThreads) {
        assert(threads_.empty());
        running_ = true;
        threads_.reserve(numThreads);
        for (int i=0; i< numThreads; ++i) {
            threads_.emplace_back(new std::thread(&ThreadPool::runInThread, this));
        }
    }

    void ThreadPool::stop() {
        running_ = false;
        cv_.notify_all();
        for (auto& t : threads_) {
            t->join();
        }
    }

    void ThreadPool::run(const ThreadPool::Task &f) {
        if (threads_.empty()) {
            f();
        } else {
            std::lock_guard<std::mutex> lockGuard(mutex_);
            tasks_.push_back(f);
            cv_.notify_one();
        }
    }


    void ThreadPool::runInThread() {
        while (running_) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                while (tasks_.empty() && running_) {
                    cv_.wait(lock);
                }
                if (!running_) {
                    break;
                }
                task = tasks_.front();
                tasks_.pop_front();
            }
            if (task) {
                task();
            }
        }
    }

}