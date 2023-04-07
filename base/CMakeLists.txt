set(base_src
    CurrentThread.cc
    Timestamp.cc
    ThreadPool.cc)

add_library(base ${base_src})
target_link_libraries(base pthread rt)

add_executable(testCurrentThread test/testCurrentThread.cc)
target_link_libraries(testCurrentThread base)

add_executable(testTimestamp test/testTimestamp.cc)
target_link_libraries(testTimestamp base)

add_executable(testThreadPool test/testThreadPool.cc)
target_link_libraries(testThreadPool base)