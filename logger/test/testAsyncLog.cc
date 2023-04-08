//
// Created by fxd on 23-4-8.
//

#include <memory>
#include <chrono>
#include <iostream>

#include "logger/AsyncLogging.h"
#include "logger/Logging.h"

int ROLL_SIZE = 500 * 1000 * 1000;
std::unique_ptr<CppNet::AsyncLogging> logging;

void asyncOut(const char* msg, int len) {
    logging->append(msg, len);
}

int main(int argc, char* argv[]) {
     logging = std::make_unique<CppNet::AsyncLogging>(::basename(argv[0]), ROLL_SIZE);
     logging->start();

     CppNet::Logger::setOutput(asyncOut);
     auto now = std::chrono::steady_clock::now();
     for (int i=0; i< 1000000; ++i) {
         LOG_INFO << "Hello 12312313";
     }
     auto end = std::chrono::steady_clock::now();
     auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - now);
     std::cout << "write 1000000 notes, spend: " << t.count() << "ms" << std::endl;
     std::this_thread::sleep_for(std::chrono::seconds(2000));
}