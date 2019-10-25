cmake_minimum_required (VERSION 3.1)
project(kernel_helper VERSION 1.0 LANGUAGES C ASM)

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

add_executable(kernel_helper src/main.c exports.c)
set_target_properties(kernel_helper 
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME "kernel_helper.prx"
)

target_link_libraries(kernel_helper
    PRIVATE
        -nostartfiles
        sdk
        utils
        syscon
        memlmd
        pspkernel
        pspsdk
)

target_include_directories(kernel_helper
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${PSP_KERNEL_LIBC}
        include
)

target_compile_definitions(kernel_helper
    PUBLIC
        KERNEL_HELPER_EXEC_NAME="kernel_helper.prx"
)

add_custom_command(OUTPUT exports.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -b ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp > exports.c
)

add_custom_command(OUTPUT kernel_helper_user.S
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -s ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
)

build_stubs(kernel_helper_user_OBJS kernel_helper_user 4)

add_library(kernel_helper_user STATIC
    ${kernel_helper_user_OBJS}
)

target_link_libraries(kernel_helper_user
    PUBLIC
        kernel_helper
)

add_custom_command(TARGET kernel_helper
    POST_BUILD
    COMMAND psp-fixup-imports kernel_helper.prx
    COMMAND psp-prxgen kernel_helper.prx kernel_helper.prx
    COMMAND psp-fixup-imports -m ${CMAKE_CURRENT_SOURCE_DIR}/661mapfile.txt kernel_helper.prx
    COMMAND $<TARGET_PROPERTY:pack_kernel_module,EXEC> --vanity=\"Things are only impossible until they're not.\" kernel_helper.prx kernel_helper.prx
)
