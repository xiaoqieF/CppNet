//
// Created by fxd on 23-4-6.
// LogStream 的单元测试
#include <gtest/gtest.h>
#include "logger/FixedBuffer.h"
#include "logger/LogStream.h"

TEST(LogStreamTest, testBooleans) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();
    EXPECT_EQ(buf.toString(), "");
    os << true;
    EXPECT_EQ(buf.toString(), "1");
    os << '\n';
    EXPECT_EQ(buf.toString(), "1\n");
    os << false;
    EXPECT_EQ(buf.toString(), "1\n0");
}

TEST(LogStreamTest, testIntegers) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();
    EXPECT_EQ(buf.toString(), "");
    os << 1;
    EXPECT_EQ(buf.toString(), "1");
    os << 0;
    EXPECT_EQ(buf.toString(), "10");
    os << -1;
    EXPECT_EQ(buf.toString(), "10-1");
    os.resetBuffer();

    os << 0 << " " << 123 << 'x' << 0x64;
    EXPECT_EQ(buf.toString(), "0 123x100");
}

TEST(LogStreamTest, testIntegerLimits) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();
    os << -2147482647;
    EXPECT_EQ(buf.toString(), "-2147482647");
    os << static_cast<int>(-2147482647 - 1);
    EXPECT_EQ(buf.toString(), "-2147482647-2147482648");
    os << ' ';
    os << 2147483647;
    EXPECT_EQ(buf.toString(), "-2147482647-2147482648 2147483647");
    os.resetBuffer();

    os << std::numeric_limits<int16_t>::min();
    EXPECT_EQ(buf.toString(), "-32768");
    os.resetBuffer();

    os << std::numeric_limits<int16_t>::max();
    EXPECT_EQ(buf.toString(), "32767");
    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::min();
    EXPECT_EQ(buf.toString(), "0");
    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::max();
    EXPECT_EQ(buf.toString(), "65535");
    os.resetBuffer();

    os << std::numeric_limits<int32_t>::min();
    EXPECT_EQ(buf.toString(), "-2147483648");
    os.resetBuffer();

    os << std::numeric_limits<int32_t>::max();
    EXPECT_EQ(buf.toString(), "2147483647");
    os.resetBuffer();

    os << std::numeric_limits<uint32_t>::min();
    EXPECT_EQ(buf.toString(), "0");
    os.resetBuffer();

    os << std::numeric_limits<uint32_t>::max();
    EXPECT_EQ(buf.toString(), "4294967295");
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::min();
    EXPECT_EQ(buf.toString(), "-9223372036854775808");
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::max();
    EXPECT_EQ(buf.toString(), "9223372036854775807");
    os.resetBuffer();

    os << std::numeric_limits<uint64_t>::min();
    EXPECT_EQ(buf.toString(), "0");
    os.resetBuffer();

    os << std::numeric_limits<uint64_t>::max();
    EXPECT_EQ(buf.toString(), "18446744073709551615");
    os.resetBuffer();

    int16_t a = 0;
    int32_t b = 0;
    int64_t c = 0;
    os << a << b << c;
    EXPECT_EQ(buf.toString(), "000");
}

TEST(LogStreamTest, testFloat) {
    using std::string;
    CppNet::LogStream os;
    const auto& buf = os.buffer();
    os << 0.0;
    EXPECT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << 1.0;
    EXPECT_EQ(buf.toString(), string("1"));
    os.resetBuffer();

    os << 0.1;
    EXPECT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    os << 0.05;
    EXPECT_EQ(buf.toString(), string("0.05"));
    os.resetBuffer();

    os << 0.15;
    EXPECT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    double a = 0.1;
    os << a;
    EXPECT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    double b = 0.05;
    os << b;
    EXPECT_EQ(buf.toString(), string("0.05"));
    os.resetBuffer();

    double c = 0.15;
    os << c;
    EXPECT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    os << a+b;
    EXPECT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    EXPECT_NE(a+b, c);

    os << 1.23456789;
    EXPECT_EQ(buf.toString(), string("1.23456789"));
    os.resetBuffer();

    os << 1.234567;
    EXPECT_EQ(buf.toString(), string("1.234567"));
    os.resetBuffer();

    os << -123.456;
    EXPECT_EQ(buf.toString(), string("-123.456"));
    os.resetBuffer();
}

TEST(LogStreamTest, testString) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();

    os << "hello ";
    EXPECT_EQ(buf.toString(), "hello ");

    std::string s = "...";
    os << s;
    EXPECT_EQ(buf.toString(), "hello ...");
}

TEST(LogStreamTest, testPtr) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();
    char str[32];
    snprintf(str, 32, "%p", &os);
    os << &os;
    EXPECT_EQ(buf.toString(), str);
}

TEST(LogStreamTest, testFmt) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();

    os << CppNet::Fmt("%4d", 1);
    EXPECT_EQ(buf.toString(), "   1");
    os.resetBuffer();

    os << CppNet::Fmt("%4.2f", 1.2);
    EXPECT_EQ(buf.toString(), "1.20");
    os.resetBuffer();

    os << CppNet::Fmt("%4.2f", 1.3) << CppNet::Fmt("%4d", 43);
    EXPECT_EQ(buf.toString(), "1.30  43");
    os.resetBuffer();
}

TEST(LogStreamTest, testLong) {
    CppNet::LogStream os;
    const auto& buf = os.buffer();
    for (int i = 0; i < 399; ++i)
    {
        os << "123456789 ";
        EXPECT_EQ(buf.length(), 10*(i+1));
        EXPECT_EQ(buf.avail(), 4000 - 10*(i+1));
    }

    os << "abcdefghi ";
    EXPECT_EQ(buf.length(), 3990);
    EXPECT_EQ(buf.avail(), 10);

    os << "abcdefghi";
    EXPECT_EQ(buf.length(), 3999);
    EXPECT_EQ(buf.avail(), 1);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}