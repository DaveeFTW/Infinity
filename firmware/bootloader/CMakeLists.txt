cmake_minimum_required (VERSION 3.1)
project(bootloader VERSION 1.0 LANGUAGES C ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_custom_command(OUTPUT exports.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp
    COMMAND psp-build-exports -b ${CMAKE_CURRENT_SOURCE_DIR}/exports.exp > exports.c
)

add_executable(bootloader660 src/main.c src/intercept.S exports.c)
add_executable(bootloader661 src/main.c src/intercept.S exports.c)

set_target_properties(bootloader660
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME "usersystemlib660.prx"
)

set_target_properties(bootloader661
    PROPERTIES 
        ENABLE_EXPORTS true
        OUTPUT_NAME "usersystemlib661.prx"
)

target_link_libraries(bootloader660
    PRIVATE
        -nostartfiles
        kexploit
        pspuser
        pspsdk
        pspctrl
        psputility
        power
        netifhandle
        vshbridge
        modulemgr
        utils
        infinityboot
)

target_link_libraries(bootloader661
    PRIVATE
        -nostartfiles
        kexploit
        pspuser
        pspsdk
        pspctrl
        psputility
        power
        netifhandle
        vshbridge
        modulemgr
        utils
        infinityboot
)

add_custom_command(TARGET bootloader660
    POST_BUILD
    COMMAND psp-fixup-imports usersystemlib660.prx
    COMMAND psp-prxgen usersystemlib660.prx usersystemlib660.prx
    COMMAND $<TARGET_PROPERTY:pack_user_fw_module,EXEC> --id=01BBB4CDC0AD6EF2C2F301711BACF4BA usersystemlib660.prx usersystemlib660.prx
)

add_custom_command(TARGET bootloader661
    POST_BUILD
    COMMAND psp-fixup-imports usersystemlib661.prx
    COMMAND psp-prxgen usersystemlib661.prx usersystemlib661.prx
    COMMAND $<TARGET_PROPERTY:pack_user_fw_module,EXEC> --id=8A425B5351139B7F13735ED533716B58 usersystemlib661.prx usersystemlib661.prx
)
