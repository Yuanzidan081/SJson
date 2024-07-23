cmake_minimum_required(VERSION 3.20)
project(SJson)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:Debug>:/MTd> # ---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> # --|
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    )
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)

    if(_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

# add_compile_options(-std=c++17)
set(CMAKE_BUILD_TYPE Debug)

message("Current build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++17 support has been enabled by default.") # 默认启用了 C++17 支持

set(TEST_ENABLE ON)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY test/libDep)
add_subdirectory("src")

if(TEST_ENABLE)
    add_subdirectory("dep/gtest")
    add_subdirectory("test")
endif()