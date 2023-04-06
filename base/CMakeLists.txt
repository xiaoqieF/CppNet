set(base_src
    CurrentThread.cc
    Timestamp.cc
    ThreadPool.cc)

add_library(base ${base_src})
target_link_libraries(base pthread rt)

add_executable(base_test1 test/test1.cc)
target_link_libraries(base_test1 base)

add_executable(base_test2 test/test2.cc)
target_link_libraries(base_test2 base)

add_executable(base_test3 test/test3.cc)
target_link_libraries(base_test3 base)