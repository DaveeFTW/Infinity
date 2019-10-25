cmake_minimum_required (VERSION 3.1)
project(libconfig VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

function(build_stubs _OBJS STUB_NAME STUB_COUNT)
    foreach(X RANGE ${STUB_COUNT})
        string(LENGTH "0000${X}" X_POSTFIX_LENGTH)
        math(EXPR X_POSTFIX_LENGTH "${X_POSTFIX_LENGTH}-4")
        string(SUBSTRING "0000${X}" ${X_POSTFIX_LENGTH} 4 X_POSTFIX)
        add_library("${STUB_NAME}_${X_POSTFIX}" OBJECT "${STUB_NAME}.S")
        target_compile_definitions("${STUB_NAME}_${X_POSTFIX}" PRIVATE "F_${STUB_NAME}_${X_POSTFIX}")
        set(OBJS ${OBJS} "$<TARGET_OBJECTS:${STUB_NAME}_${X_POSTFIX}>")
    endforeach()
    set(${_OBJS} ${OBJS} PARENT_SCOPE)
endfunction()

add_library(libconfig_static OBJECT src/config.c)
add_executable(libconfig src/main.c src/config.c exports.c)

set_target_properties(libconfig 
    PROPERTIES
        ENABLE_EXPORTS true
        OUTPUT_NAME "libconfig.prx"
)

target_link_libraries(libconfig_static
    INTERFACE
        pspkernel
        pspsdk
)

target_link_libraries(libconfig
    PRIVATE
        -nostartfiles
        pspkernel
        pspsdk
)

target_include_directories(libconfig_static
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${PSP_KERNEL_LIBC}
        include
)

target_include_directories(libconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${PSP_KERNEL_LIBC}
        include
)

target_compile_definitions(libconfig
    PUBLIC
        LIBCONFIG_EXEC_NAME="libconfig.prx"
)

add_custom_command(OUTPUT exports.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -b ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp > exports.c
)

add_custom_command(OUTPUT libconfig_kernel.S libconfig_user.S
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -s ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
)

build_stubs(libconfig_kernel_OBJS libconfig_kernel 12)
build_stubs(libconfig_user_OBJS libconfig_user 12)

add_library(libconfig_kernel STATIC
    ${libconfig_kernel_OBJS}
)

target_include_directories(libconfig_kernel
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(libconfig_user STATIC
    ${libconfig_user_OBJS}
)

target_include_directories(libconfig_user
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_custom_command(TARGET libconfig
    POST_BUILD
    COMMAND psp-fixup-imports libconfig.prx
    COMMAND psp-prxgen libconfig.prx libconfig.prx
    COMMAND psp-fixup-imports -m ${CMAKE_CURRENT_SOURCE_DIR}/661mapfile.txt libconfig.prx
    COMMAND $<TARGET_PROPERTY:pack_kernel_module,EXEC> --vanity=\"Things are only impossible until they're not.\" libconfig.prx libconfig.prx
)
