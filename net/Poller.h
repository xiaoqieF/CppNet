//
// Created by fxd on 23-4-9.
//

#ifndef CPPNET_POLLER_H
#define CPPNET_POLLER_H

#include <vector>
#include "base/Noncopyable.h"
#include "base/Timestamp.h"

namespace CppNet {
    class EventLoop;
    class Channel;

    class Poller : noncopyable{
    public:
        typedef std::vector<Channel*> ChannelList;
        explicit Poller(EventLoop* loop);
        virtual ~Poller();

        virtual Timestamp poll(int timeouts, ChannelList* activeChannels) = 0;
        virtual void updateChannel(Channel* channel) = 0;
        virtual void removeChannel(Channel* channel) = 0;
        void assertInLoopThread();

    private:
        EventLoop* ownerLoop_;
    };

}

#endif //CPPNET_POLLER_H
