# Copyright (c) 2019, David "Davee" Morgan
# Copyright (c) 2016, Yifan Lu
# Based off of Android toolchain file
# Copyright (c) 2010-2011, Ethan Rublee
# Copyright (c) 2011-2014, Andrey Kamaev
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1.  Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
# 2.  Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# 3.  Neither the name of the copyright holder nor the names of its
#     contributors may be used to endorse or promote products derived from this
#     software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required( VERSION 2.6.3 )

if( DEFINED CMAKE_CROSSCOMPILING )
  # subsequent toolchain loading is not really needed
  return()
endif()

if( CMAKE_TOOLCHAIN_FILE )
  # touch toolchain variable to suppress "unused variable" warning
endif()

set( CMAKE_SYSTEM_NAME Generic )
set( CMAKE_SYSTEM_VERSION 1 )

# search for PSP SDK path 1) where this toolchain file is, 2) in environment var, 3) manually defined
if( NOT DEFINED ENV{PSPDEV} )
  get_filename_component(__pspdev_path ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
  if( EXISTS "${__pspdev_path}" )
    set( PSPDEV "${__pspdev_path}" )
    set( ENV{PSPDEV} "${__pspdev_path}" )
  endif()
else()
  set( PSPDEV "$ENV{PSPDEV}" )
endif()
set( PSPDEV "${PSPDEV}" CACHE PATH "Path to PSP SDK root" )

if( NOT EXISTS "${PSPDEV}" )
  message( FATAL_ERROR "Cannot find PSPDEV at ${PSPDEV}" )
endif()

set( TOOL_OS_SUFFIX "" )
if( CMAKE_HOST_WIN32 )
 set( TOOL_OS_SUFFIX ".exe" )
endif()

set( CMAKE_SYSTEM_PROCESSOR "PSP" )

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${PSPDEV}/bin/psp-gcc${TOOL_OS_SUFFIX}" GCC)
CMAKE_FORCE_CXX_COMPILER("${PSPDEV}/bin/psp-g++${TOOL_OS_SUFFIX}" GCC)

set( CMAKE_C_COMPILER   "${PSPDEV}/bin/psp-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "C compiler" )
set( CMAKE_CXX_COMPILER "${PSPDEV}/bin/psp-g++${TOOL_OS_SUFFIX}"     CACHE PATH "C++ compiler" )
set( CMAKE_ASM_COMPILER "${PSPDEV}/bin/psp-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "assembler" )
set( CMAKE_STRIP        "${PSPDEV}/bin/psp-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" )
set( CMAKE_AR           "${PSPDEV}/bin/psp-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" )
set( CMAKE_LINKER       "${PSPDEV}/bin/psp-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" )
set( CMAKE_NM           "${PSPDEV}/bin/psp-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" )
set( CMAKE_OBJCOPY      "${PSPDEV}/bin/psp-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" )
set( CMAKE_OBJDUMP      "${PSPDEV}/bin/psp-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" )
set( CMAKE_RANLIB       "${PSPDEV}/bin/psp-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" )

# cache flags
set( CMAKE_CXX_FLAGS           "-I${PSPDEV}/psp/sdk/include -Os -Wall -std=c++1y -G0 -mno-gpopt -fno-pic -fshort-wchar -D_PSP_FW_VERSION=660 -fomit-frame-pointer"                          CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS             "-I${PSPDEV}/psp/sdk/include -Os -Wall -std=gnu99 -G0 -mno-gpopt -fno-pic -fshort-wchar -D_PSP_FW_VERSION=660 -fomit-frame-pointer"           CACHE STRING "c flags" )
set( CMAKE_ASM_FLAGS "-I${PSPDEV}/psp/sdk/include ${CMAKE_ASM_FLAGS}" CACHE STRING "asm flags")
set( CMAKE_CXX_FLAGS_RELEASE   "-Os -DNDEBUG"                 CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE     "-Os -DNDEBUG"                 CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -DDEBUG -D_DEBUG"      CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG       "-O0 -g -DDEBUG -D_DEBUG"      CACHE STRING "c Debug flags" )
set( CMAKE_SHARED_LINKER_FLAGS ""                             CACHE STRING "shared linker flags" )
set( CMAKE_MODULE_LINKER_FLAGS ""                             CACHE STRING "module linker flags" )
set( CMAKE_EXE_LINKER_FLAGS    "-L${PSPDEV}/psp/sdk/lib -specs=${PSPDEV}/psp/sdk/lib/prxspecs -Wl,-q,-T${PSPDEV}/psp/sdk/lib/linkfile.prx" CACHE STRING "executable linker flags" )

set(PSP_KERNEL_LIBC ${PSPDEV}/psp/sdk/include/libc CACHE STRING "kernel libc")

# set these global flags for cmake client scripts to change behavior
set( PSP True )
set( BUILD_PSP True )

# where is the target environment
set( CMAKE_FIND_ROOT_PATH "${PSPDEV}/bin" "${PSPDEV}/psp" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share" )
set( CMAKE_INSTALL_PREFIX "${PSPDEV}/psp" CACHE PATH "default install path" )

# only search for libraries and includes in psp toolchain
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
