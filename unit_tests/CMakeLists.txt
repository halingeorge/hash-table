cmake_minimum_required(VERSION 3.8)

enable_testing()
include_directories(
    ../src
    ../googletest
    ../googletest/include
)

add_executable(
    hash_table_test
    thread_local_test.cpp
    rcu_lock_test.cpp
    hash_table_test.cpp
    hash_table_stress_test.cpp
)

target_compile_features(hash_table_test PUBLIC cxx_std_17)
set_target_properties(hash_table_test PROPERTIES COMPILE_FLAGS "-stdlib=libc++ -pthread")
target_link_libraries(hash_table_test gtest_main)
