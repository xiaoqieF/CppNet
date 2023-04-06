//
// Created by fxd on 23-4-5.
// 测试 CurrentThread.h 中的 __thread 变量在不同线程独立
#include <thread>
#include <iostream>
#include <mutex>

#include "base/CurrentThread.h"

std::mutex mutex;

void threadFunc() {
    std::lock_guard<std::mutex> lock(mutex);
    auto tid = CppNet::CurrentThread::tid();
    std::cout << "threadID is: " << tid << std::endl;
}

int main() {
    auto t1 =  std::thread(threadFunc);
    auto t2 = std::thread(threadFunc);
    t1.join();
    t2.join();
    std::cout << "main threadID: " << CppNet::CurrentThread::tid() << std::endl;
}