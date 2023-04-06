//
// Created by fxd on 23-4-6.
//
#include "logger/Logging.h"
#include <iostream>

int main() {
    CppNet::Logger::setLogLevel(CppNet::Logger::TRACE);
    LOG_TRACE << "hello";
    return 0;
}