//
// Created by fxd on 23-4-5.
//

#ifndef CPPNET_CURRENTTHREAD_H
#define CPPNET_CURRENTTHREAD_H

namespace CppNet {
    namespace CurrentThread {
        extern __thread int t_cachedTid;
        extern __thread char t_tidString[32];
        extern __thread int t_tidLen;

        void cacheTid();

        inline int tid() {
            if (t_cachedTid == 0) {
                cacheTid();
            }
            return t_cachedTid;
        }

        inline const char* tidString() {
            return t_tidString;
        }
    }
}

#endif //CPPNET_CURRENTTHREAD_H
