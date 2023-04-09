//
// Created by fxd on 23-4-9.
//

#include "PollPoller.h"
#include "logger/Logging.h"
#include "net/Channel.h"

namespace CppNet {

    void PollPoller::removeChannel(Channel *channel) {
        assertInLoopThread();
        LOG_TRACE << "fd = " << channel->fd();
        int idx = channel->index();
        channels_.erase(channel->fd());
        if (static_cast<size_t>(idx) == pollFds_.size()-1) {
            pollFds_.pop_back();
        } else {
            std::iter_swap(pollFds_.begin() + idx, pollFds_.end()-1);
            channels_[pollFds_[idx].fd]->setIndex(idx);
            pollFds_.pop_back();
        }
    }

    void PollPoller::updateChannel(Channel *channel) {
        assertInLoopThread();
        LOG_TRACE << "fd = " << channel->fd() << "events = " << channel->events();
        if (channel->index() < 0) {
            // new channel
            struct pollfd pfd;
            pfd.fd = channel->fd();
            pfd.events = static_cast<short>(channel->events());
            pfd.revents = 0;
            pollFds_.push_back(pfd);
            int idx = static_cast<int>(pollFds_.size()) - 1;
            channel->setIndex(idx);
            channels_.insert({pfd.fd, channel});
        } else {
            // update channel
            int idx = channel->index();
            auto& pfd = pollFds_[idx];
            pfd.events = static_cast<short>(channel->events());
            pfd.revents = 0;
        }
    }

    Timestamp PollPoller::poll(int timeouts, Poller::ChannelList *activeChannels) {
        int numEvents = ::poll(&*pollFds_.begin(), pollFds_.size(), timeouts);
        Timestamp now = Timestamp::now();
        if (numEvents > 0) {
            LOG_TRACE << numEvents << " events happened";
            // 将已发生事件的 Channel 找出来
            fillActiveChannels(numEvents, activeChannels);
        } else if (numEvents == 0) {
            LOG_TRACE << "nothing happened";
        } else {
            LOG_FATAL << "PollPoller::poll";
        }
        return now;
    }

    void PollPoller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const {
        for (const auto& p : pollFds_) {
            if (numEvents <= 0) {
                break;
            }
            if (p.revents > 0) {
                --numEvents;
                auto it = channels_.find(p.fd);
                assert(it != channels_.end());
                Channel* channel = it->second;
                channel->set_revents(p.revents);
                activeChannels->push_back(channel);
            }
        }
    }
}