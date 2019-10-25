cmake_minimum_required (VERSION 3.1)
project(compat_interface VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(compat_interface INTERFACE)

target_include_directories(compat_interface
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(compat_interface
    INTERFACE
        sdk
)

target_compile_definitions(compat_interface
    INTERFACE
        COMPATIBILITY_MODULE_ADDR=0x88F50000
)
