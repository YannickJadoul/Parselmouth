# Copyright (C) 2016-2019  Yannick Jadoul
#
# This file is part of Parselmouth.
#
# Parselmouth is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Parselmouth is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>

cmake_minimum_required(VERSION 3.3)
project(Parselmouth)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/cmake ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/installed CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif ()

if (WIN32)
	set(PYBIND11_CPP_STANDARD "/std:c++latest")
endif ()

add_subdirectory(pybind11)

if (PREBUILT_PRAAT_DIR)
	file(TO_CMAKE_PATH ${PREBUILT_PRAAT_DIR} PREBUILT_PRAAT_DIR_CMAKE)
	include(${PREBUILT_PRAAT_DIR_CMAKE}/exports.cmake)
	set(CMAKE_NO_SYSTEM_FROM_IMPORTED 1) # Get -I instead of -isystem, when importing an already-built praat lib
else ()
	add_subdirectory(praat)
endif ()

add_subdirectory(src)
add_subdirectory(tests)
