cmake_minimum_required(VERSION 3.8)

enable_testing()
include_directories(
    ../lib
    ../googletest
    ../googletest/include
)

add_executable(unittests unittests.cpp thread_local_test.cpp)
target_link_libraries(unittests gtest_main)
