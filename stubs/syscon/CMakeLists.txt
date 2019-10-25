cmake_minimum_required (VERSION 3.1)
project(syscon VERSION 1.0 LANGUAGES ASM)

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

build_stubs(sceSyscon_driver_OBJS sceSyscon_driver 1)

add_library(syscon STATIC
    ${sceSyscon_driver_OBJS}
)

target_include_directories(syscon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
