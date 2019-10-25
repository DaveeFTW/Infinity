cmake_minimum_required (VERSION 3.1)
project(rebootex VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXE_LINKER_FLAGS "")

set (REBOOTEX_SOURCES
    "src/crt0.S"
    "src/btcnf.c"
    "src/btcnfpatch.c"
    "src/compatibility.c"
    "src/kprintf.c"
    "src/lfatpatch.c"
    "src/libc.c"
    "src/loadcorepatch.c"
    "src/main.c"
    "src/psp_uart.c"
    "src/syscon.c"
    "src/sysreg.c"
    "src/utility.c"
)

add_library(rebootex_interface INTERFACE)

target_compile_definitions(rebootex_interface
    INTERFACE
        REBOOTEX_ADDRESS=0x88F10000
)

target_include_directories(rebootex_interface
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(rebootex_interface
    INTERFACE
        sdk
)

add_executable(rebootex ${REBOOTEX_SOURCES})

set_target_properties(rebootex 
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME rebootex.bin
        COMPILE_FLAGS "-Wno-unused-but-set-variable"
)

target_link_libraries(rebootex
    PRIVATE
        rebootex_interface
        compat_interface
        utils
        -nostartfiles
        -nostdlib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/linkfile.l
)

target_compile_definitions(rebootex
    PUBLIC
        REBOOTEX_EXEC_PATH="flash0:/kd/rebootex.bin"
    PRIVATE
        INFINITY_OE_TAG=${INFINITY_OE_TAG}
)

add_custom_command(TARGET rebootex
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary rebootex.bin rebootex.bin
)
