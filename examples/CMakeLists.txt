add_executable(echo echo.cc)
target_link_libraries(echo net)

add_executable(daytime daytime.cc)
target_link_libraries(daytime net)