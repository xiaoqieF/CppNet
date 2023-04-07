//
// Created by fxd on 23-4-7.
//

#include <memory>
#include <unistd.h>

#include "logger/LogFile.h"
#include "logger/Logging.h"

std::unique_ptr<CppNet::LogFile> file;

void outputFunc(const char* msg, int len) {
    file->append(msg, len);
}

void flushFunc() {
    file->flush();
}

int main(int argc, const char* argv[]) {
    file = std::make_unique<CppNet::LogFile>(::basename(argv[0]), 4000 * 1000);
    CppNet::Logger::setOutput(outputFunc);
    CppNet::Logger::setFlush(flushFunc);

    std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for (int i = 0; i < 10000; ++i)
    {
        LOG_INFO << line << i;

        usleep(1000);
    }
}