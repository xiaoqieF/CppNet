//
// Created by fxd on 23-4-13.
//

#include "TcpConnection.h"

#include <utility>
#include <unistd.h>
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Socket.h"
#include "logger/Logging.h"

namespace CppNet {

    TcpConnection::TcpConnection(EventLoop *loop,
                                 std::string name,
                                 int sockfd,
                                 const InetAddress &localAddr,
                                 const InetAddress &peerAddr)
        : loop_(checkLoopNotNull(loop)),
          name_(std::move(name)),
          state_(kConnecting),
          channel_(new Channel(loop, sockfd)),
          socket_(new Socket(sockfd)),
          localAddr_(localAddr),
          peerAddr_(peerAddr),
          highWaterMark_(64 * 1024 * 1024)
   {
        channel_->setReadCallback([this](Timestamp t) {
            handleRead(t);
        });
        channel_->setWriteCallback([this]() {
            handleWrite();
        });
        channel_->setCloseCallback([this]() {
            handleClose();
        });
        channel_->setErrorCallback([this]() {
            handleError();
        });
        LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
                  << " fd=" << sockfd;
        socket_->setKeepAlive(true);
   }

    TcpConnection::~TcpConnection() {
        LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
                  << " fd=" << channel_->fd();
    }

    void TcpConnection::send(const void *message, size_t len) {
        if (state_ == kConnected) {
            if (loop_->isInLoopThread()) {
                sendInLoop(message, len);
            } else {
                // FIXME: 这里 this 指针 和 message 指针悬空问题
                loop_->runInLoop([this, message, len]() {
                    sendInLoop(message, len);
                });
            }
        }
    }

    void TcpConnection::send(const std::string &message) {
        if (state_ == kConnected) {
            if (loop_->isInLoopThread()) {
                sendInLoop(message.c_str(), message.size());
            } else {
                // FIXME: this 指针 和 引用捕获悬空问题
                loop_->runInLoop([this, &message]() {
                    sendInLoop(message.c_str(), message.size());
                });
            }
        }
    }

    void TcpConnection::send(Buffer *buf) {
        if (state_ == kConnected) {
            if (loop_->isInLoopThread()) {
                sendInLoop(buf->peek(), buf->readableBytes());
                buf->retrieveAll();
            } else {
                loop_->runInLoop([this, buf]() {
                    sendInLoop(buf->peek(), buf->readableBytes());
                });
            }
        }
    }

    void TcpConnection::shutdown() {
        if (state_ == kConnected) {
            state_ = kDisconnecting;
            // FIXME: this ?
            loop_->runInLoop([this]() {
                shutdownInLoop();
            });
        }
    }

    void TcpConnection::shutdownInLoop() {
        loop_->assertInLoopThread();
        if (!channel_->isWriting()) {
            socket_->shutdownWrite();
        }
    }

    void TcpConnection::setTcpNoDelay(bool on) {
        socket_->setTcpNoDelay(on);
    }

    void TcpConnection::sendInLoop(const void *message, size_t len) {
        loop_->assertInLoopThread();
        ssize_t wrote = 0;
        size_t remaining = len;
        // 如果缓冲区没有待发数据，直接调用 write 写一次数据
        if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
            wrote = ::write(channel_->fd(), message, len);
            if (wrote >= 0) {
                remaining = len - wrote;
                if (remaining == 0 && writeCompleteCallback_) {
                    loop_->queueInLoop([this]() {
                        writeCompleteCallback_(shared_from_this());
                    });
                }
            } else {
                wrote = 0;
                if (errno != EWOULDBLOCK) {
                    /// FIXME: SIGPIPE
                    LOG_ERROR << "TcpConnection::sendInLoop";
                }
            }
        }
        assert(remaining <= len);
        // 一次没写完
        if (remaining > 0) {
            LOG_TRACE << "write more data";
            size_t oldLen = outputBuffer_.readableBytes();
            // 高水位
            if (oldLen + remaining >= highWaterMark_
                && oldLen < highWaterMark_ && highWaterMarkCallback_) {
                loop_->queueInLoop([this, cur = oldLen+remaining]() {
                    highWaterMarkCallback_(shared_from_this(), cur);
                });
            }
            outputBuffer_.append(static_cast<const char*>(message) + wrote, remaining);
            if (!channel_->isWriting()) {
                channel_->enableWriting();      // 关注 POLLOUT 事件
            }
        }
    }

    void TcpConnection::handleRead(Timestamp receiveTime) {
        loop_->assertInLoopThread();
        int saveErrno = 0;
        ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
        if (n > 0) {
            messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
        } else if (n == 0) {
            handleClose();
        } else {
            errno = saveErrno;
            LOG_ERROR << "TcpConnection::handleRead";
            handleError();
        }
    }

    void TcpConnection::handleWrite() {
        loop_->assertInLoopThread();
        if (channel_->isWriting()) {
            ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
            if (n > 0) {
                outputBuffer_.retrieve(n);
                if (outputBuffer_.readableBytes() == 0) {
                    channel_->disableWriting();
                    if (writeCompleteCallback_) {
                        loop_->queueInLoop([this]() {
                            writeCompleteCallback_(shared_from_this());
                        });
                    }
                    if (state_ == kDisconnecting) {
                        shutdownInLoop();
                    }
                } else {
                    LOG_TRACE << "write more data";
                }
            } else {
                LOG_ERROR << "TcpConnection::handleWrite";
            }
        } else {
            LOG_TRACE << "Connection is down, no more writing";
        }
    }

    void TcpConnection::handleClose() {
        loop_->assertInLoopThread();
        LOG_TRACE << "TcpConnection::handleClose state = " << state_;
        state_ = kDisconnected;
        channel_->disableAll();
        connectionCallback_(shared_from_this());
        closeCallback_(shared_from_this());
    }

    void TcpConnection::handleError() {
        int err = 0;
        socklen_t len = sizeof(err);
        if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &err, &len)) {
            err = errno;
        }
        LOG_ERROR << "TcpConnection::handleError [" << name_
                  << "] - SO_ERROR = " << err << " " << strerror_tl(err);
    }

    void TcpConnection::connectEstablished() {
        loop_->assertInLoopThread();
        assert(state_ == kConnecting);
        state_ = kConnected;
        channel_->tie(shared_from_this());   // 防止 TcpConnection 被销毁了， channel_ 还在处理事件
        channel_->enableReading();

        connectionCallback_(shared_from_this());
    }

    void TcpConnection::connectDestroyed() {
        loop_->assertInLoopThread();
        if (state_ == kConnected) {
            state_ = kDisconnected;
            channel_->disableAll();
            connectionCallback_(shared_from_this());
        }
        channel_->remove();
    }

}