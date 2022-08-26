cmake_minimum_required (VERSION 3.1)
project(infinity-configuration VERSION 1.0 LANGUAGES C CXX ASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set (CONFIGURATION_SOURCES
    src/animations.cpp
    src/application.cpp
    src/backgroundview.cpp
    src/blockallocator.cpp
    src/buttonarbiter.cpp
    src/buttonevent.cpp
    src/buttontransition.cpp
    src/colouredrectangle.cpp
    src/compatibilityconfig.cpp
    src/compatibilitymodule.cpp
    src/configscreen.cpp
    src/creditsscreen.cpp
    src/errorscreen.cpp
    src/event.cpp
    src/eventsource.cpp
    src/exitscreen.cpp
    src/firework.cpp
    src/flashingrectangle.cpp
    src/font.cpp
    src/fontgen.cpp
    src/fontmanager.cpp
    src/functortransition.cpp
    src/gltext.cpp
    src/firework.cpp
    src/graphicsdevice.cpp
    src/homescreen.cpp
    src/infinityintro.cpp
    src/installscreen.cpp
    src/installupdate.cpp
    src/installupdatescreen.cpp
    src/localupdatescreen.cpp
    src/main.cpp
    src/menu.cpp
    src/menuselectionevent.cpp
    src/menuselectiontransition.cpp
    src/mutex.cpp
    src/mutexlocker.cpp
    src/numberanimation.cpp
    src/pageview.cpp
    src/particlesource.cpp
    src/pspguwrapper.cpp
    src/randomgenerator.cpp
    src/rectangle.cpp
    src/scene.cpp
    src/snowscene.cpp
    src/state.cpp
    src/statemachine.cpp
    src/stateview.cpp
    src/tailedparticle.cpp
    src/textrenderer.cpp
    src/texture.cpp
    src/tgatexture.cpp
    src/thread.cpp
    src/timer.cpp
    src/transition.cpp
    src/updateinformation.cpp
    src/updateinformationparser.cpp
    src/uptodatescreen.cpp
    src/utility.cpp
    src/view.cpp
    src/viewmanager.cpp
    src/vram.c
)

add_subdirectory(3rdparty/intraFont-G)

add_custom_command(OUTPUT generated_file_data.h generated_file_data.cpp
    DEPENDS libconfig kernel_helper bootloader660 bootloader661 infinityboot infinityctrl rebootex mecompat procompat arkcompat
    COMMAND $<TARGET_PROPERTY:bin2cpp,EXEC>
        libconfig="$<TARGET_FILE:libconfig>"
        kernel_helper="$<TARGET_FILE:kernel_helper>"
        bootloader660="$<TARGET_FILE:bootloader660>"
        bootloader661="$<TARGET_FILE:bootloader661>"
        infinityboot="$<TARGET_FILE:infinityboot>"
        infinityctrl="$<TARGET_FILE:infinityctrl>"
        rebootex="$<TARGET_FILE:rebootex>"
        mecompat="$<TARGET_FILE:mecompat>"
        procompat="$<TARGET_FILE:procompat>"
        arkcompat="$<TARGET_FILE:arkcompat>"
        parallax_left=${CMAKE_CURRENT_SOURCE_DIR}/resources/parallaxleft.tga
        parallax_right=${CMAKE_CURRENT_SOURCE_DIR}/resources/parallaxright.tga
        symbol_font=${CMAKE_CURRENT_SOURCE_DIR}/resources/symbol.pgf
        generated_file_data.h generated_file_data.cpp
)

add_executable(configuration ${CONFIGURATION_SOURCES} generated_file_data.cpp)
target_link_libraries(configuration
    intrafont
    utils
    libconfig_user
    kernel_helper_user
    libinfinityUser
    infinityboot
    power
    vshbridge
    kexploit
    stdc++
    c
    pspvfpu
    pspgum
    pspgu
    psprtc
    m
    psphttp
    pspssl
    pspdebug
    pspdisplay
    pspge
    pspctrl
    pspsdk
    c
    pspnet
    pspnet_inet
    pspnet_apctl
    pspnet_resolver
    psputility
    pspuser
    pspkernel

    infinityboot
    infinityctrl
    rebootex
    mecompat
    procompat
    arkcompat
)

target_include_directories(configuration PRIVATE 3rdparty ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(configuration
    PRIVATE
        INFINITY_VERSION="${infinity_VERSION}"
        INFINITY_VERSION_MAJOR=${infinity_VERSION_MAJOR}
        INFINITY_VERSION_MINOR=${infinity_VERSION_MINOR}
        INFINITY_VERSION_PATCH=${infinity_VERSION_PATCH}
)

add_custom_command(TARGET configuration
    POST_BUILD
    COMMAND psp-fixup-imports configuration
    COMMAND psp-prxgen configuration configuration.prx
    COMMAND pack-pbp EBOOT.PBP ${CMAKE_CURRENT_SOURCE_DIR}/resources/PARAM.SFO ${CMAKE_CURRENT_SOURCE_DIR}/resources/icon0.png NULL NULL NULL NULL configuration.prx NULL
    COMMAND pack-pbp EBOOT_GO.PBP ${CMAKE_CURRENT_SOURCE_DIR}/resources/PARAM_GO.SFO ${CMAKE_CURRENT_SOURCE_DIR}/resources/icon0.png NULL NULL NULL NULL configuration.prx NULL
    COMMAND $<TARGET_PROPERTY:pack_updater,EXEC> --vanity=\" Infinite keys open infinite doors.\" EBOOT.PBP EBOOT.PBP
    COMMAND $<TARGET_PROPERTY:pack_updater,EXEC> --vanity=\" Infinite keys open infinite doors.\" EBOOT_GO.PBP EBOOT_GO.PBP
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/icon0.png
    BYPRODUCTS EBOOT.PBP
    BYPRODUCTS EBOOT_GO.PBP
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EBOOT.PBP
    DESTINATION standard
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EBOOT_GO.PBP
    DESTINATION pspgo
    RENAME EBOOT.PBP
)
