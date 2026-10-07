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

message(STATUS "Configuring CPack...")

set(CPACK_PACKAGE_VENDOR "The QElectroTech Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/License.txt")

list(LENGTH CPACK_GENERATOR cpack_generator_count)
if(cpack_generator_count EQUAL 0)
  message(
    STATUS "CPACK_GENERATOR not specified, no CPack package will be generated")
else()
  foreach(generator ${CPACK_GENERATOR})
    if(generator STREQUAL "NSIS")
      include(nsis.cmake)
    elseif(generator STREQUAL "WIX")
      include(wix.cmake)
    elseif(generator STREQUAL "ZIP")
      include(zip.cmake)
    elseif(generator STREQUAL "TGZ")
      include(tgz.cmake)
    elseif(generator STREQUAL "DEB")
      include(deb.cmake)
    elseif(generator STREQUAL "RPM")
      include(rpm.cmake)
    else()
      message(STATUS "Unsupported CPack generator: ${generator}")
      list(REMOVE_ITEM CPACK_GENERATOR "${generator}")
      math(EXPR cpack_generator_count "${cpack_generator_count} - 1")
    endif()
  endforeach()
  if(cpack_generator_count GREATER 0)
    message(STATUS "Generating CPack package(s) for ${CPACK_GENERATOR}")
    include(CPack)
  else()
    message(
      STATUS "No supported CPack generator specified, no CPack package will be"
      " generated")
  endif()
endif()
