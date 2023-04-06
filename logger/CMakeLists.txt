set(log_src FixedBuffer.h LogStream.cc Logging.cc)

add_library(logger ${log_src})
set_target_properties(logger PROPERTIES LINKER_LANGUAGE CXX)

add_executable(logger_test1 test/test1.cc)
target_link_libraries(logger_test1 logger gtest gmock pthread)

add_executable(logger_test2 test/test2.cc)
target_link_libraries(logger_test2 logger gtest gmock pthread base)