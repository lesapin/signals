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

# CMakeList.txt : CMake project for sample_extension, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(signals "extension.cpp" "../smsdk_ext.cpp")

add_custom_target(
    echo ALL COMMAND ${CMAKE_COMMAND} -E echo ${MY_INCLUDE}
)

set(MY_INCLUDE ${MY_INCLUDE} . ..)

target_include_directories(signals PUBLIC ${MY_INCLUDE})

target_link_libraries(signals PUBLIC -lm -m32 "-Wl,--exclude-libs,ALL" -shared -static-libgcc)

# TODO: Add tests and install targets if needed.
