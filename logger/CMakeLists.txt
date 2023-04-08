set(log_src FixedBuffer.h LogStream.cc Logging.cc LogFile.cc AsyncLogging.cc)

add_library(logger ${log_src})
target_link_libraries(logger base)
set_target_properties(logger PROPERTIES LINKER_LANGUAGE CXX)

add_executable(testLogStream test/testLogStream.cc)
target_link_libraries(testLogStream logger gtest gmock pthread)

add_executable(testLogging test/testLogging.cc)
target_link_libraries(testLogging logger gtest gmock pthread)

add_executable(testLogFile test/testLogFile.cc)
target_link_libraries(testLogFile logger)

add_executable(testAsyncLog test/testAsyncLog.cc)
target_link_libraries(testAsyncLog logger)