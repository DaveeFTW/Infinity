cmake_minimum_required (VERSION 3.1)
project(stubs VERSION 1.0)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(power)
add_subdirectory(syscon)
add_subdirectory(vshbridge)
add_subdirectory(netifhandle)
add_subdirectory(memlmd)
add_subdirectory(modulemgr)
