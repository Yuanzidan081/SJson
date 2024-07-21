cmake_minimum_required(VERSION 3.20)

project(SJsonTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++17 support has been enabled by default.")

set(GTEST "${CMAKE_SOURCE_DIR}/test/gcheck.cpp" "${CMAKE_SOURCE_DIR}/test/gtest.cpp")
message("the gtest files path is: ${CMAKE_SOURCE_DIR}")

add_executable(SJsonGTest ${GTEST})
target_link_libraries(SJsonGTest gtest SJsonApp)

set(TEST "${CMAKE_SOURCE_DIR}/test/test.cpp")
add_executable(SJsonTest ${TEST})
target_link_libraries(SJsonTest SJsonApp)