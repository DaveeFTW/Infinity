cmake_minimum_required (VERSION 3.13.4)
project(sdk VERSION 1.0)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

add_library(sdk INTERFACE)

target_link_directories(sdk
    BEFORE
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
)

target_include_directories(sdk
    BEFORE
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
