//
// Created by fxd on 23-4-8.
//

#include <gtest/gtest.h>

#include "net/InetAddress.h"

TEST(testInetAddress, test) {
    CppNet::InetAddress addr(1234);
    EXPECT_EQ(addr.toIp(), "0.0.0.0");
    EXPECT_EQ(addr.toIpPort(), "0.0.0.0:1234");

    CppNet::InetAddress addr1("1.2.3.4", 8080);
    EXPECT_EQ(addr1.toIp(), "1.2.3.4");
    EXPECT_EQ(addr1.toIpPort(), "1.2.3.4:8080");

    CppNet::InetAddress addr2("255.255.255.255", 65535);
    EXPECT_EQ(addr2.toIp(), "255.255.255.255");
    EXPECT_EQ(addr2.toIpPort(), "255.255.255.255:65535");
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}