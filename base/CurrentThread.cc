//
// Created by fxd on 23-4-5.
//

#include <unistd.h>
#include <sys/syscall.h>
#include "CurrentThread.h"

namespace CppNet {
    namespace CurrentThread {
        __thread int t_cachedTid = 0;

        void cacheTid() {
            if (t_cachedTid == 0) {
                t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
            }
        }
    }
}