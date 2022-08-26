cmake_minimum_required (VERSION 3.1)
project(arkcompat VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXE_LINKER_FLAGS "")

set (ARKCOMPAT_SOURCES
    "src/crt0.S"
    "src/main.c"
)

add_executable(arkcompat ${ARKCOMPAT_SOURCES})

set_target_properties(arkcompat 
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME arkcompat.bin
)

target_link_libraries(arkcompat
    PRIVATE
        compat_interface
        utils
        sdk
        -nostartfiles
        -nostdlib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/linkfile.l
)

add_custom_command(TARGET arkcompat
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary arkcompat.bin arkcompat.bin
)

target_compile_definitions(arkcompat
    PUBLIC
        ARKCOMPAT_EXEC_PATH="flash0:/kd/arkcompat.bin"
)
