cmake_minimum_required(VERSION 3.22.1)

find_package(GTest REQUIRED)

add_executable(scoped_ptr_test scoped_ptr_unittest.cc)

target_link_libraries(scoped_ptr_test GTest::GTest GTest::Main)

