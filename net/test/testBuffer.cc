//
// Created by fxd on 23-4-9.
//

#include <gtest/gtest.h>
#include "net/Buffer.h"

using namespace CppNet;

TEST(testBuffer, appendRetrieve) {
    CppNet::Buffer buffer;
    EXPECT_EQ(buffer.readableBytes(), 0);
    EXPECT_EQ(buffer.writableBytes(), Buffer::kInitialSize);
    EXPECT_EQ(buffer.prependBytes(), Buffer::kCheapPrepend);

    const std::string str(200, 'x');
    buffer.append(str);
    EXPECT_EQ(buffer.readableBytes(), str.size());
    EXPECT_EQ(buffer.writableBytes(), Buffer::kInitialSize - str.size());
    EXPECT_EQ(buffer.prependBytes(), Buffer::kCheapPrepend);

    const std::string str2 =  buffer.retrieveAsString(50);
    EXPECT_EQ(str2.size(), 50);
    EXPECT_EQ(buffer.readableBytes(), str.size() - str2.size());
    EXPECT_EQ(buffer.writableBytes(), Buffer::kInitialSize - str.size());
    EXPECT_EQ(buffer.prependBytes(), Buffer::kCheapPrepend + str2.size());

    buffer.append(str);
    EXPECT_EQ(buffer.readableBytes(), 2*str.size() - str2.size());
    EXPECT_EQ(buffer.writableBytes(), Buffer::kInitialSize - 2*str.size());
    EXPECT_EQ(buffer.prependBytes(), Buffer::kCheapPrepend + str2.size());

    const std::string str3 =  buffer.retrieveAllAsString();
    EXPECT_EQ(str3.size(), 350);
    EXPECT_EQ(buffer.readableBytes(), 0);
    EXPECT_EQ(buffer.writableBytes(), Buffer::kInitialSize);
    EXPECT_EQ(buffer.prependBytes(), Buffer::kCheapPrepend);
}

TEST(testBuffer, glow) {
    Buffer buf;
    buf.append(std::string(400, 'y'));
    EXPECT_EQ(buf.readableBytes(), 400);
    EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-400);

    buf.retrieve(50);
    EXPECT_EQ(buf.readableBytes(), 350);
    EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-400);
    EXPECT_EQ(buf.prependBytes(), Buffer::kCheapPrepend+50);

    buf.append(std::string(1000, 'z'));
    EXPECT_EQ(buf.readableBytes(), 1350);
    EXPECT_EQ(buf.writableBytes(), 0);
    EXPECT_EQ(buf.prependBytes(), Buffer::kCheapPrepend+50);

    buf.retrieveAll();
    EXPECT_EQ(buf.readableBytes(), 0);
    EXPECT_EQ(buf.writableBytes(), 1400);
    EXPECT_EQ(buf.prependBytes(), Buffer::kCheapPrepend);
}

TEST(testBuffer, insideMove) {
    Buffer buf;
    buf.append(std::string(800, 'y'));
    EXPECT_EQ(buf.readableBytes(), 800);
    EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-800);

    buf.retrieve(500);
    EXPECT_EQ(buf.readableBytes(), 300);
    EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-800);
    EXPECT_EQ(buf.prependBytes(), Buffer::kCheapPrepend+500);

    // copy to kCheapPrepend
    buf.append(std::string(300, 'z'));
    EXPECT_EQ(buf.readableBytes(), 600);
    EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-600);
    EXPECT_EQ(buf.prependBytes(), Buffer::kCheapPrepend);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}