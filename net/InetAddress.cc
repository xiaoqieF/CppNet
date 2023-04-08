//
// Created by fxd on 23-4-8.
//

#include <cstring>
#include "InetAddress.h"
#include "logger/Logging.h"

namespace CppNet {

    InetAddress::InetAddress(const std::string& ip, uint16_t port) {
        bzero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0) {
            LOG_ERROR << "InetAddress::InetAddress";
        }
    }

    InetAddress::InetAddress(uint16_t port) {
        bzero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    std::string InetAddress::toIp() const {
        char buf[32];
        inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
        return buf;
    }

    std::string InetAddress::toIpPort() const {
        char buf[32];
        inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
        size_t host_len = strlen(buf);
        uint16_t port = ntohs(addr_.sin_port);
        snprintf(buf + host_len, sizeof(buf) - host_len, ":%u", port);
        return buf;
    }
}