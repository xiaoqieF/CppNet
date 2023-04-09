//
// Created by fxd on 23-4-9.
//

#ifndef CPPNET_POLLPOLLER_H
#define CPPNET_POLLPOLLER_H

#include <vector>
#include <poll.h>
#include <map>

#include "net/Poller.h"

namespace CppNet {
    class EventLoop;
    class Channel;

    class PollPoller : public Poller {
    public:
        explicit PollPoller(EventLoop* loop) : Poller(loop) {}
        ~PollPoller() override = default;

        Timestamp poll(int timeouts, ChannelList *activeChannels) override;
        void removeChannel(Channel *channel) override;
        void updateChannel(Channel *channel) override;

    private:
        void fillActiveChannels(int n, ChannelList* activeChannels) const;
        std::vector<struct pollfd> pollFds_;
        std::map<int, Channel*> channels_;
    };
}


#endif //CPPNET_POLLPOLLER_H
