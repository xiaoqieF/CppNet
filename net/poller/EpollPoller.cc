//
// Created by fxd on 23-4-9.
//

#include <unistd.h>

#include "EpollPoller.h"
#include "logger/Logging.h"
#include "net/Channel.h"

namespace CppNet {
    // index of channel， 用来标志 channel 的行为
    const int kNew = -1;   // 新 channel，不在 epfd_ 中 且不在 channels_ 中
    const int kAdded = 1;  // 已经添加的 channel
    const int kDeleted = 2; // 删除的 channel， 不在 epfd_ 中， 但在 channels_ 中

    EpollPoller::EpollPoller(EventLoop *loop_)
        : Poller(loop_),
          epfd_(::epoll_create1(EPOLL_CLOEXEC)),
          events_(kInitEventListSize)
        {}

    EpollPoller::~EpollPoller() {
        ::close(epfd_);
    }

    void EpollPoller::updateChannel(Channel *channel) {
        assertInLoopThread();
        LOG_TRACE << "updateChannel, fd = " << channel->fd() << " events = " << channel->events();
        const int idx = channel->index();
        if (idx == kNew || idx == kDeleted) {
            // new channel
            int fd = channel->fd();
            if (idx == kNew) {
                channels_.insert({fd, channel});
            }
            channel->setIndex(kAdded);
            update(EPOLL_CTL_ADD, channel);
        } else {
            if (channel->isNoneEvent()) {
                update(EPOLL_CTL_DEL, channel);
                channel->setIndex(kDeleted);
            } else {
                update(EPOLL_CTL_MOD, channel);
            }
        }
    }

    void EpollPoller::removeChannel(Channel *channel) {
        assertInLoopThread();
        int fd = channel->fd();
        LOG_TRACE << "fd = " << fd;
        int idx = channel->index();
        channels_.erase(fd);
        if (idx == kAdded) {
            update(EPOLL_CTL_DEL, channel);
        }
        channel->setIndex(kNew);
    }

    Timestamp EpollPoller::poll(int timeouts, Poller::ChannelList *activeChannels) {
        int numEvents = ::epoll_wait(epfd_, &*events_.begin(),
                                     static_cast<int>(events_.size()), timeouts);
        Timestamp now = Timestamp::now();
        if (numEvents > 0) {
            LOG_TRACE << numEvents << " events happened";
            fillActiveChannels(numEvents, activeChannels);
            if (static_cast<size_t>(numEvents) == events_.size()) {
                events_.resize(events_.size() * 2);
            }
        } else if (numEvents == 0) {
            LOG_TRACE << "nothing happened";
        } else {
            if (errno != EINTR) {
                LOG_FATAL << "EpollPoller::poll()";
            }
        }
        return now;
    }

    void EpollPoller::update(int operation, Channel* channel) {
        struct epoll_event event;
        bzero(&event, sizeof(event));
        event.events = channel->events();
        event.data.ptr = channel;
        int fd = channel->fd();
        if (::epoll_ctl(epfd_, operation, fd, &event) < 0) {
            LOG_FATAL << "epoll_ctl, op=" << operation << " fd=" <<fd;
        }
    }

    void EpollPoller::fillActiveChannels(int numEvent, Poller::ChannelList *active) const {
        for (int i=0; i<numEvent; ++i) {
            auto* channel = static_cast<Channel*> (events_[i].data.ptr);
            channel->set_revents(static_cast<int>(events_[i].events));
            active->push_back(channel);
        }
    }

}