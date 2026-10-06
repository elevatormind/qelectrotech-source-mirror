# Copyright 2006-2026 The QElectroTech Team
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

# Paths used for compilation and installation of QET

message(" - paths_compilation_installation")

include(GNUInstallDirs)

set(QET_INSTALL_PREFIX          "${CMAKE_INSTALL_PREFIX}")
set(QET_BINARY_PATH             "${CMAKE_INSTALL_BINDIR}")
set(QET_ICONS_PATH              "${CMAKE_INSTALL_DATADIR}/icons/hicolor/")

if(UNIX AND NOT APPLE)
  # for Linux, BSD, Solaris, Minix
  set(QET_COMMON_COLLECTION_PATH  "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/elements")
  set(QET_COMMON_TBT_PATH         "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/titleblocks")
  set(QET_LANG_PATH               "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/lang")
  set(QET_EXAMPLES_PATH           "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/examples")
  set(QET_DOC_PATH                "${CMAKE_INSTALL_DATADIR}/doc/${QET_PACKAGE_NAME}")
  set(QET_MCP_PATH                "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/mcp")
  set(QET_MIME_PACKAGE_PATH       "${CMAKE_INSTALL_DATADIR}/mime/packages")
  set(QET_DESKTOP_PATH            "${CMAKE_INSTALL_DATADIR}/applications/")
  set(QET_MAN_PATH                "${CMAKE_INSTALL_MANDIR}")
  set(QET_APPDATA_PATH            "${CMAKE_INSTALL_DATADIR}/appdata")
  endif()

if(APPLE)
  # for MacOS X or iOS, watchOS, tvOS (since 3.10.3)
  set(QET_COMMON_COLLECTION_PATH  "../Resources/elements/")
  set(QET_COMMON_TBT_PATH         "../Resources/titleblocks/")
  set(QET_LANG_PATH               "../Resources/lang/")
  set(QET_EXAMPLES_PATH           "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/examples")
  set(QET_DOC_PATH                "${CMAKE_INSTALL_DATADIR}/doc/${QET_PACKAGE_NAME}")
  set(QET_MCP_PATH                "${CMAKE_INSTALL_DATADIR}/${QET_PACKAGE_NAME}/mcp")
  set(QET_DESKTOP_PATH            "${CMAKE_INSTALL_DATADIR}/applications/")
  set(QET_MAN_PATH                "${CMAKE_INSTALL_MANDIR}")
  set(ICON                        "ico/mac_icon/qelectrotech.icns")
endif()

if(WIN32)
  # for Windows operating system in general
  # Paths are relative to CMAKE_INSTALL_PREFIX (no CMAKE_INSTALL_DATADIR)
  # to match the layout expected by qet_mcp.py's _installation():
  # <prefix>/mcp/ -> <prefix>/bin, <prefix>/elements
  set(QET_COMMON_COLLECTION_PATH  "elements")
  set(QET_COMMON_TBT_PATH         "titleblocks")
  set(QET_LANG_PATH               "lang")
  set(QET_EXAMPLES_PATH           "examples")
  set(QET_DOC_PATH                "doc")
  set(QET_MCP_PATH                "mcp")
  # Liste des ressources Windows
  #RC_FILE = qelectrotech.rc
endif()

# cmake_path() added in CMake 3.20
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
  cmake_path(NORMAL_PATH QET_INSTALL_PREFIX)
  cmake_path(NORMAL_PATH QET_BINARY_PATH)
  cmake_path(NORMAL_PATH QET_ICONS_PATH)
  cmake_path(NORMAL_PATH QET_COMMON_COLLECTION_PATH)
  cmake_path(NORMAL_PATH QET_COMMON_TBT_PATH)
  cmake_path(NORMAL_PATH QET_LANG_PATH)
  cmake_path(NORMAL_PATH QET_EXAMPLES_PATH)
  cmake_path(NORMAL_PATH QET_DOC_PATH)
  cmake_path(NORMAL_PATH QET_MCP_PATH)

  if(QET_MIME_PACKAGE_PATH)
    cmake_path(NORMAL_PATH QET_MIME_PACKAGE_PATH)
  endif()

  if(QET_DESKTOP_PATH)
    cmake_path(NORMAL_PATH QET_DESKTOP_PATH)
  endif()

  if(QET_MAN_PATH)
    cmake_path(NORMAL_PATH QET_MAN_PATH)
  endif()

  if(QET_APPDATA_PATH)
    cmake_path(NORMAL_PATH QET_APPDATA_PATH)
  endif()
endif()
