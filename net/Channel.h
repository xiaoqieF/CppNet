//
// Created by fxd on 23-4-9.
//

#ifndef CPPNET_CHANNEL_H
#define CPPNET_CHANNEL_H

#include <functional>
#include <memory>

#include "base/Noncopyable.h"
#include "base/Timestamp.h"

namespace CppNet {
    class EventLoop;

    class Channel : noncopyable{
    public:
        typedef std::function<void()> EventCallback;
        typedef std::function<void(Timestamp )> ReadEventCallback;

        Channel(EventLoop* loop, int fd);
        ~Channel() = default;

        void handleEvent(Timestamp receiveTime);
        void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
        void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
        void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }
        void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        void set_revents(int revent) { revents_ = revent; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }

        void tie(const std::shared_ptr<void>&);
        void enableReading() {
            events_ |= kReadEvent;
            update();
        }
        void enableWriting() {
            events_ |= kWriteEvent;
            update();
        }
        void disableWriting() {
            events_ &= ~kWriteEvent;
            update();
        }
        void disableAll() {
            events_ = kNoneEvent;
            update();
        }
        bool isWriting() const { return events_ & kWriteEvent; }

        // for poller
        int index() const { return index_; }
        void setIndex(int idx) { index_ = idx; }

        EventLoop* ownerLoop() { return loop_; }
        void remove();

        std::string reventsToString() const;

    private:
        void update();
        void handleEventWithGuard(Timestamp receiveTime);

        EventLoop* loop_;
        const int fd_;
        int events_;
        int revents_;
        int index_;    // 保存自己在 poller 的 fd 列表中的索引(或当前状态)，便于查找更新当前 Channel

        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;

        std::weak_ptr<void> tie_;   // 用于保存 Channel 持有者的弱引用，防止在处理事件时持有者被销毁
        bool tied_;

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
    };

}

#endif //CPPNET_CHANNEL_H
