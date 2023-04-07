//
// Created by fxd on 23-4-5.
// Timestamp
#include <iostream>
#include "base/Timestamp.h"

int main() {
    using namespace CppNet;
    auto t = Timestamp::now();
    std::cout << "now: " << t.toFormatString() << std::endl;
    std::cout << "now: " << t.toString() << std::endl;
}