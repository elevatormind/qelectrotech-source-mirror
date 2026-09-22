# Copyright 2006 The QElectroTech Team
# This file is part of QElectroTech.
#
# QElectroTech is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# QElectroTech is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.

if(QET_AUTODETECT_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        if(DEFINED CMAKE_C_COMPILER_LAUNCHER)
            message(WARNING
                "CMAKE_C_COMPILER_LAUNCHER will be overridden by "
                "QET_AUTODETECT_CCACHE")
        endif()
        if(DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
            message(WARNING
                "CMAKE_CXX_COMPILER_LAUNCHER will be overridden by "
                "QET_AUTODETECT_CCACHE")
        endif()
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else()
        message(WARNING
            "QET_AUTODETECT_CCACHE is enabled, but ccache was not found")
    endif()
endif()

if(DEFINED CMAKE_C_COMPILER_LAUNCHER)
    message(STATUS
        "Using CMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}")
else()
    message(STATUS
        "CMAKE_C_COMPILER_LAUNCHER is not set, ccache will not be used")
endif()

if(DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
    message(STATUS
        "Using CMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}")
else()
    message(STATUS
        "CMAKE_CXX_COMPILER_LAUNCHER is not set, ccache will not be used")
endif()
