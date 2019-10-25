cmake_minimum_required (VERSION 3.1)
project(infinityboot VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(infinityboot "src/main.c" exports.c)

set_target_properties(infinityboot 
    PROPERTIES
        ENABLE_EXPORTS true
        OUTPUT_NAME infinityboot.prx
)

target_link_libraries(infinityboot
    PRIVATE
        -nostartfiles
        rebootex_interface
        compat_interface
        libconfig_static
        reboot
        utils
        rebootex
        sdk
        pspkernel
        pspctrl_driver
        pspsdk
)

target_include_directories(infinityboot
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${PSP_KERNEL_LIBC}
        include
)

target_compile_definitions(infinityboot
    PUBLIC
        INFINITYBOOT_EXEC_PATH="flash0:/kd/infinityboot.prx"
)

add_custom_command(OUTPUT exports.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -b ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp > exports.c
)

add_custom_command(TARGET infinityboot
    POST_BUILD
    COMMAND psp-fixup-imports infinityboot.prx
    COMMAND psp-prxgen infinityboot.prx infinityboot.prx
    COMMAND psp-fixup-imports -m ${CMAKE_CURRENT_SOURCE_DIR}/661mapfile.txt infinityboot.prx
    COMMAND $<TARGET_PROPERTY:pack_kernel_module,EXEC> --vanity=\"Things are only impossible until they're not.\" infinityboot.prx infinityboot.prx
)
