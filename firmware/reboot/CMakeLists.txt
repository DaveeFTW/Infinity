cmake_minimum_required (VERSION 3.1)
project(reboot VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set (REBOOT_SOURCES
    "src/reboot.c"
    "src/utility.c"
)

add_library(reboot STATIC ${REBOOT_SOURCES})

target_link_libraries(reboot
    PUBLIC
        rebootex_interface
        compat_interface
        libconfig_static

    PRIVATE
        rebootex_interface
        compat_interface
        libconfig_static
        pspkernel
        sdk
        utils
        rebootex
)

target_include_directories(reboot
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${PSP_KERNEL_LIBC}
        include
)
