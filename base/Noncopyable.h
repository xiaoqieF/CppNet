//
// Created by fxd on 23-4-5.
//

#ifndef CPPNET_NONCOPYABLE_H
#define CPPNET_NONCOPYABLE_H

namespace CppNet {
    class noncopyable {
    public:
        noncopyable(const noncopyable& ) = delete;
        noncopyable& operator=(const noncopyable& ) = delete;
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };
}

#endif //CPPNET_NONCOPYABLE_H
