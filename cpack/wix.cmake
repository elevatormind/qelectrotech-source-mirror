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

message(STATUS "Generating WIX installer...")

set(CPACK_WIX_VERSION 4)

set(CPACK_WIX_UPGRADE_GUID "A1B2C3D4-E5F6-7890-ABCD-EF1234567890")

set(ICON_PATH "${QET_DIR}/ico/windows_icon/qelectrotech.ico")
set(CPACK_WIX_PRODUCT_ICON "${ICON_PATH}")

string(REPLACE "/" "." QET_BINARY_PATH_DOT "${QET_BINARY_PATH}")
string(REPLACE "\\" "." QET_BINARY_PATH_DOT "${QET_BINARY_PATH_DOT}")

configure_file(
    "${QET_DIR}/cpack/wix/shortcuts.wxs.in"
    "${CMAKE_BINARY_DIR}/cpack/wix/shortcuts.wxs"
    @ONLY)

configure_file(
    "${QET_DIR}/LICENSE"
    "${CMAKE_BINARY_DIR}/License.txt"
    COPYONLY)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/License.txt")

list(APPEND CPACK_WIX_PATCH_FILE "${CMAKE_BINARY_DIR}/cpack/wix/shortcuts.wxs")
