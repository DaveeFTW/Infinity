cmake_minimum_required (VERSION 3.1)
project(infinityctrl VERSION 1.0 LANGUAGES C ASM)

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

set (INFINITYCTRL_SOURCES
    "src/libinfinity.c"
    "src/main.c"
    "src/infinity_ver.S"
)

add_executable(infinityctrl ${INFINITYCTRL_SOURCES} exports.c)

set_target_properties(infinityctrl 
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME infinityctrl.prx
)

target_link_libraries(infinityctrl
    PRIVATE
        -nostartfiles
        reboot
        utils
        sdk
        pspkernel
        pspsdk
)

target_include_directories(infinityctrl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${PSP_KERNEL_LIBC}
        include
)

target_compile_definitions(infinityctrl
    PUBLIC
        INFINITYCTRL_EXEC_PATH="flash0:/kd/infinityctrl.prx"
    PRIVATE
        INFINITY_VERSION="${infinity_VERSION}"
        INFINITY_VERSION_MAJOR=${infinity_VERSION_MAJOR}
        INFINITY_VERSION_MINOR=${infinity_VERSION_MINOR}
        INFINITY_VERSION_PATCH=${infinity_VERSION_PATCH}
)

add_custom_command(OUTPUT exports.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -b ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp > exports.c
)

add_custom_command(OUTPUT libinfinityKernel.S libinfinityUser.S
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -s ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
)

add_custom_command(TARGET infinityctrl
    POST_BUILD
    COMMAND psp-fixup-imports infinityctrl.prx
    COMMAND psp-prxgen infinityctrl.prx infinityctrl.prx
    COMMAND psp-fixup-imports -m ${CMAKE_CURRENT_SOURCE_DIR}/661mapfile.txt infinityctrl.prx
    COMMAND $<TARGET_PROPERTY:pack_kernel_module,EXEC> --vanity=\"Things are only impossible until they're not.\" infinityctrl.prx infinityctrl.prx
)

# handle exports
build_stubs(libinfinityKernel_OBJS libinfinityKernel 3)
build_stubs(libinfinityUser_OBJS libinfinityUser 3)

add_library(libinfinityKernel STATIC
    ${libinfinityKernel_OBJS}
)

target_link_libraries(libinfinityKernel
    PUBLIC
        infinityctrl
)

add_library(libinfinityUser STATIC
    ${libinfinityUser_OBJS}
)

target_link_libraries(libinfinityUser
    PUBLIC
        infinityctrl
)
