//
// Created by fxd on 23-4-9.
//

#ifndef CPPNET_EVENTLOOP_H
#define CPPNET_EVENTLOOP_H

#include <atomic>
#include <memory>
#include <mutex>

#include "base/Noncopyable.h"
#include "base/Timestamp.h"
#include "base/CurrentThread.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include "net/timer/TimerQueue.h"

namespace CppNet {
    class EventLoop {
    public:
        typedef std::function<void()> Functor;
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

        // 被 Channel 调用，进而调用 poller 的相关方法
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);

        // 在事件循环中立即执行函数 f(添加到 pendingFunc 中，wakeup 循环，并执行函数)
        void runInLoop(const Functor& f);
        void queueInLoop(const Functor& f);

        // 向 wakeupFd 发送信息，唤醒事件循环
        void wakeup() const;

        // 通过 TimerQueue 实现定时效果
        void runAt(Timestamp t, const Functor& f);
        void runAfter(double delay, const Functor& f);
        void runEvery(double interval, const Functor& f);

    private:
        void abortNotInLoopThread();
        void handleWakeup() const;
        void doPendingFunctors();

        std::atomic_bool looping_;
        std::atomic_bool quit_;
        const pid_t threadId_;

        std::unique_ptr<Poller> poller_;
        Timestamp pollReturnTime_;
        std::vector<Channel*> activeChannels_;     // 当前有事件发生的 Channel

        std::unique_ptr<TimerQueue> timerQueue_;   // 使用 timerfd 和红黑树实现的时间队列

        int wakeupFd_;                             // 使用 eventfd 实现的事件循环唤醒机制
        std::unique_ptr<Channel> wakeupChannel_;

        std::mutex functorMutex_;
        std::vector<Functor> pendingFunctors_;     // 事件循环中等待执行的函数(醒来立即执行)
        std::atomic_bool callingPendingFunctors_;
    };

}

#endif //CPPNET_EVENTLOOP_H
