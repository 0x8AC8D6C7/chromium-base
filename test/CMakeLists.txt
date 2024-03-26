cmake_minimum_required(VERSION 3.16)

find_package(GTest REQUIRED)

include_directories(../src)

add_subdirectory(scoped_ptr_unittest)

macro(add_gtest_executable TARGET_NAME SRC_FILE)
    add_executable(${TARGET_NAME} ${SRC_FILE})
    target_link_libraries(${TARGET_NAME} GTest::GTest GTest::Main base)
endmacro()

file(GLOB TEST_SOURCES "src/*_unittest.cc")

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE) # 从完整路径中提取文件名（无扩展名）
    add_gtest_executable(${TEST_NAME} ${TEST_SOURCE})
endforeach()

# add_executable(base_time_unittest src/base_time_unittest.cc)
# add_executable(bits_unittest src/bits_unittest.cc)
# add_executable(md5_unittest src/md5_unittest.cc)
# add_executable(to_delete src/to_delete.cc)
# add_executable(at_exit_unittest src/at_exit_unittest.cc)

# target_link_libraries(base_time_unittest GTest::GTest GTest::Main base)
# target_link_libraries(bits_unittest GTest::GTest GTest::Main base)
# target_link_libraries(md5_unittest GTest::GTest GTest::Main base)
# target_link_libraries(to_delete GTest::Main base)
# target_link_libraries(at_exit_unittest GTest::Main base)
