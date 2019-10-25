cmake_minimum_required (VERSION 3.1)
project(recovery VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXE_LINKER_FLAGS "")

set (RECOVERY_SOURCES
    "src/crt0.S"
    "src/main.c"
)

add_executable(recovery ${RECOVERY_SOURCES})

set_target_properties(recovery 
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME recovery.bin
)

target_link_libraries(recovery
    PRIVATE
        utils
        sdk
        -nostartfiles
        -nostdlib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/linkfile.l
)

add_custom_command(TARGET recovery
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary recovery.bin recovery.bin
)
