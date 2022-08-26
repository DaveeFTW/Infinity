cmake_minimum_required (VERSION 3.1)

project(infinity VERSION 2.0.5)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/psp-toolchain.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(tools)
add_subdirectory(utils)
add_subdirectory(sdk)
add_subdirectory(stubs)
add_subdirectory(kexploit)
add_subdirectory(kernel_helper)
add_subdirectory(libconfig)
add_subdirectory(firmware)
add_subdirectory(app)

install(FILES README.md LICENSE
    DESTINATION .
)

set(CPACK_GENERATOR "ZIP")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
include(CPack)
