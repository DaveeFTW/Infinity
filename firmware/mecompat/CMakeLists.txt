cmake_minimum_required (VERSION 3.1)
project(mecompat VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXE_LINKER_FLAGS "")

set (MECOMPAT_SOURCES
    "src/crt0.S"
    "src/main.c"
)

add_executable(mecompat ${MECOMPAT_SOURCES})

set_target_properties(mecompat 
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME mecompat.bin
)

target_link_libraries(mecompat
    PRIVATE
        compat_interface
        utils
        sdk
        -nostartfiles
        -nostdlib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/linkfile.l
)

target_compile_definitions(mecompat
    PUBLIC
        MECOMPAT_EXEC_PATH="flash0:/kd/mecompat.bin"
)

add_custom_command(TARGET mecompat
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary mecompat.bin mecompat.bin
)
