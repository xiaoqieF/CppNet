set(net_src Socket.cc InetAddress.cc)

add_library(net ${net_src})
target_link_libraries(net logger base)

add_executable(testInetAddress test/testInetAddress.cc)
target_link_libraries(testInetAddress net gtest gmock pthread)