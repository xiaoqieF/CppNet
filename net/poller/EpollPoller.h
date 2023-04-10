//
// Created by fxd on 23-4-9.
//

#ifndef CPPNET_EPOLLPOLLER_H
#define CPPNET_EPOLLPOLLER_H

#include <vector>
#include <map>
#include <sys/epoll.h>

#include "net/Poller.h"

namespace CppNet {
    class Channel;
    class EventLoop;

    class EpollPoller : public Poller {
    public:
        explicit EpollPoller(EventLoop* loop_);
        ~EpollPoller() override;

        void updateChannel(Channel *channel) override;
        void removeChannel(Channel *channel) override;
        Timestamp poll(int timeouts, ChannelList *activeChannels) override;
    private:
        static const int kInitEventListSize = 16;
        void update(int operation, Channel* channel);
        void fillActiveChannels(int numEvent, ChannelList* activeChannels) const;

        int epfd_;
        std::vector<epoll_event> events_;   // 用来存已经发生的事件
        std::map<int, Channel*> channels_;
    };

}

#endif //CPPNET_EPOLLPOLLER_H
