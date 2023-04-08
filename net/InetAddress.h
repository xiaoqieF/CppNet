//
// Created by fxd on 23-4-8.
//

#ifndef CPPNET_INETADDRESS_H
#define CPPNET_INETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

namespace CppNet {
    class InetAddress {
    public:
        InetAddress(const std::string& ip, uint16_t port);
        InetAddress(const sockaddr_in& addr) : addr_(addr) {}
        explicit InetAddress(uint16_t port);

        std::string toIp() const;
        std::string toIpPort() const;

        const struct sockaddr_in& getSockAddrIn() const {
            return addr_;
        }
        void setSockAddrIn(const sockaddr_in& addr) {
            addr_ = addr;
        }

    private:
        sockaddr_in addr_;
    };
}

#endif //CPPNET_INETADDRESS_H
