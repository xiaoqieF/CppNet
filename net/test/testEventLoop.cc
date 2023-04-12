//
// Created by fxd on 23-4-9.
//

#include <thread>
#include <vector>
#include <unistd.h>

#include "base/CurrentThread.h"
#include "logger/Logging.h"
#include "net/EventLoopThread.h"
#include "net/EventLoopThreadPool.h"
#include "net/EventLoop.h"

using namespace CppNet;
void print(EventLoop* p = nullptr)
{
    printf("main(): pid = %d, tid = %d, loop = %p\n",
           getpid(), CurrentThread::tid(), p);
}

void init(EventLoop* p)
{
    printf("init(): pid = %d, tid = %d, loop = %p\n",
           getpid(), CurrentThread::tid(), p);
}

int main() {
    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);
    print();
    EventLoop loop;
//    loop.runAfter(10, [&loop] { print(&loop); });

//    {
//        printf("Single thread %p:\n", &loop);
//        EventLoopThreadPool model(&loop);
//        model.setThreadNum(0);
//        model.start(init);
//        assert(model.getNextLoop() == &loop);
//        assert(model.getNextLoop() == &loop);
//        assert(model.getNextLoop() == &loop);
//    }

    {
        printf("Another thread:\n");
        EventLoopThreadPool model(&loop);
        model.setThreadNum(1);
        model.start(init);
        EventLoop* nextLoop = model.getNextLoop();
        nextLoop->runAfter(5, [nextLoop] { return print(nextLoop); });
        assert(nextLoop != &loop);
        assert(nextLoop == model.getNextLoop());
        assert(nextLoop == model.getNextLoop());
        ::sleep(3);
    }

//    {
//        printf("Three threads:\n");
//        EventLoopThreadPool model(&loop);
//        model.setThreadNum(3);
//        model.start(init);
//        EventLoop* nextLoop = model.getNextLoop();
//        nextLoop->runInLoop([nextLoop] { return print(nextLoop); });
//        assert(nextLoop != &loop);
//        assert(nextLoop != model.getNextLoop());
//        assert(nextLoop != model.getNextLoop());
//        assert(nextLoop == model.getNextLoop());
//    }

    loop.loop();
}