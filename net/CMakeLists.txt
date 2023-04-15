set(net_src
        Socket.cc
        InetAddress.cc
        Buffer.cc
        EventLoop.cc
        Channel.cc
        Poller.cc
        poller/PollPoller.cc
        poller/EpollPoller.cc
        timer/Timer.cc
        timer/TimerQueue.cc
        EventLoopThread.cc
        EventLoopThreadPool.cc
        TcpServer.cc
        Acceptor.cc
        TcpConnection.cc)

add_library(net ${net_src})
target_link_libraries(net logger base)

add_executable(testInetAddress test/testInetAddress.cc)
target_link_libraries(testInetAddress net gtest gmock pthread)

add_executable(testBuffer test/testBuffer.cc)
target_link_libraries(testBuffer net gtest gmock pthread)

add_executable(testEventLoop test/testEventLoop.cc)
target_link_libraries(testEventLoop net)

add_executable(testAcceptor test/testAcceptor.cc)
target_link_libraries(testAcceptor net)

add_executable(testTcpServer test/testTcpServer.cc)
target_link_libraries(testTcpServer net)