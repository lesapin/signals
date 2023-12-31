#
# Signal Forwards Extension
# Copyright (C) 2024 bezdmn
# =============================================================================
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License, version 3.0, as published by the
# Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

project ("signals"	VERSION 1.0
                    DESCRIPTION "POSIX Signals Extension for SourceMod"
                    LANGUAGES CXX)

set(RELATIVE_ROOT ".")
get_filename_component(ROOT ${RELATIVE_ROOT} ABSOLUTE)

set(SMSDK   "${ROOT}/sourcemod")
set(HL2SDK  "${ROOT}/hl2sdk-tf2")
set(HL2PUB  "${HL2SDK}/public")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14 -fpic -m32 -fno-exceptions -fno-threadsafe-statics -fvisibility-inlines-hidden \
                                        -Wno-overloaded-virtual -Wno-non-virtual-dtor -Wno-delete-non-virtual-dtor")

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pipe -fpic -m32 -fno-strict-aliasing -Wall -Werror -Wno-unused -Wno-switch -Wno-array-bounds \
                                    -msse -m32 -fvisibility=hidden -Wno-unused-result -Wno-narrowing -mfpmath=sse")

# target_compile_definitions() to target a specific extension
add_compile_definitions("GNUC -D_LINUX -DPOSIX -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_snprintf=snprintf \
                         -D_vsnprintf=vsnprintf -DHAVE_STDINT_H")

# configured for extensions
set(MY_INCLUDE "${SMSDK}/public" "${SMSDK}/public/extensions" "${SMSDK}/sourcepawn/include" 
               "${SMSDK}/public/amtl/amtl" "${SMSDK}/public/amtl")

# Include sub-projects.
add_subdirectory ("sourcemod/public/signals_extension")
