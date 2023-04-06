//
// Created by fxd on 23-4-5.
//

#ifndef CPPNET_CURRENTTHREAD_H
#define CPPNET_CURRENTTHREAD_H

namespace CppNet {
    namespace CurrentThread {
        extern __thread int t_cachedTid;

        void cacheTid();

        inline int tid() {
            if (t_cachedTid == 0) {
                cacheTid();
            }
            return t_cachedTid;
        }
    }
}

#endif //CPPNET_CURRENTTHREAD_H
