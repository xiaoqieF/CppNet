//
// Created by fxd on 23-4-5.
// 测试 ThreadPool

#include <thread>
#include <iostream>
#include <chrono>
#include "base/CurrentThread.h"
#include "base/ThreadPool.h"

std::mutex outMutex;

void consumer() {
    static const int COUNTS = 10;
    for (int i=0; i<COUNTS; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::lock_guard<std::mutex> lockGuard(outMutex);  // 防止输出混乱
    std::cout << "Thread: " << CppNet::CurrentThread::tid() << " consume a task" << std::endl;
}

int main() {
    using namespace CppNet;
    ThreadPool pool;
    pool.start(8);
    for (int i=0; i<8; ++i) {
        pool.run(consumer);
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    pool.stop();
}