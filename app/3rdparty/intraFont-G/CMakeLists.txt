cmake_minimum_required (VERSION 3.1)
project(intraFont-G VERSION 1.0 LANGUAGES C)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

link_directories(lib)

add_library(intrafont intraFont.c libccc.c)

target_link_libraries(intrafont
    c
    pspvfpu
    pspgum
    pspgu
    psprtc
    m
    pspdebug
    pspdisplay
    pspge
    pspctrl
    pspsdk
    c
    psputility
    pspuser
    pspkernel
)

target_include_directories(intrafont
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        include
)
