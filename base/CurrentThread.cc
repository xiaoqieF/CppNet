//
// Created by fxd on 23-4-5.
//

#include <unistd.h>
#include <sys/syscall.h>
#include <string>
#include <cassert>
#include "CurrentThread.h"

namespace CppNet {
    namespace CurrentThread {
        __thread int t_cachedTid = 0;
        __thread char t_tidString[32];
        __thread int t_tidLen = 0;     // length of t_tidString

        void cacheTid() {
            if (t_cachedTid == 0) {
                t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
                t_tidLen = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
            }
        }
    }
}